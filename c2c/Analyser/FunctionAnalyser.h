/* Copyright 2013-2016 Bas van den Berg
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ANALYSER_FUNCTION_ANALYSER_H
#define ANALYSER_FUNCTION_ANALYSER_H

#include <string>
#include <vector>
#include <llvm/ADT/APSInt.h>
#include <clang/Basic/SourceLocation.h>
#include "AST/Type.h"
#include "Analyser/ExprTypeAnalyser.h"

namespace clang {
class DiagnosticsEngine;
class DiagnosticBuilder;
}

namespace C2 {

class TypeResolver;
class Scope;
class Decl;
class VarDecl;
class FunctionDecl;
class EnumConstantDecl;
class LabelDecl;
class Stmt;
class SwitchStmt;
class Expr;
class IdentifierExpr;
class InitListExpr;
class MemberExpr;

class FunctionAnalyser {
public:
    FunctionAnalyser(Scope& scope_,
                    TypeResolver& typeRes_,
                    TypeContext& tc,
                    clang::DiagnosticsEngine& Diags_,
                    bool isInterface_);

    unsigned check(FunctionDecl* F);
    unsigned checkVarInit(VarDecl* V);
    unsigned checkArraySizeExpr(VarDecl* V);
    unsigned checkEnumValue(EnumConstantDecl* E, llvm::APSInt& nextValue);
private:
    void checkFunction(FunctionDecl* F);

    void analyseStmt(Stmt* stmt, bool haveScope = false);
    void analyseCompoundStmt(Stmt* stmt);
    void analyseIfStmt(Stmt* stmt);
    void analyseWhileStmt(Stmt* stmt);
    void analyseDoStmt(Stmt* stmt);
    void analyseForStmt(Stmt* stmt);
    void analyseSwitchStmt(Stmt* stmt);
    void analyseBreakStmt(Stmt* S);
    void analyseContinueStmt(Stmt* S);
    void analyseLabelStmt(Stmt* S);
    void analyseGotoStmt(Stmt* S);
    void analyseCaseStmt(Stmt* stmt);
    void analyseDefaultStmt(Stmt* stmt);
    void analyseReturnStmt(Stmt* stmt);
    void analyseDeclStmt(Stmt* stmt);
    void analyseCondition(Stmt* stmt);
    void analyseStmtExpr(Stmt* stmt);

    QualType analyseExpr(Expr* expr, unsigned side);
    QualType analyseIntegerLiteral(Expr* expr);
    QualType analyseBinaryOperator(Expr* expr, unsigned side);
    QualType analyseConditionalOperator(Expr* expr);
    QualType analyseUnaryOperator(Expr* expr, unsigned side);
    QualType analyseBuiltinExpr(Expr* expr);
    QualType analyseArraySubscript(Expr* expr, unsigned side);
    QualType analyseMemberExpr(Expr* expr, unsigned side);
    QualType analyseStructMember(QualType T, MemberExpr* M, unsigned side);
    QualType analyseParenExpr(Expr* expr);
    bool analyseBitOffsetIndex(Expr* expr, llvm::APSInt* Result, BuiltinType* BaseType);
    QualType analyseBitOffsetExpr(Expr* expr, QualType BaseType, clang::SourceLocation base);
    QualType analyseExplicitCastExpr(Expr* expr);
    QualType analyseCall(Expr* expr);
    Decl* analyseIdentifier(IdentifierExpr* expr);

    void analyseInitExpr(Expr* expr, QualType expectedType);
    void analyseInitList(InitListExpr* expr, QualType expectedType);
    void analyseDesignatorInitExpr(Expr* expr, QualType expectedType);
    void analyseSizeofExpr(Expr* expr);
    void analyseArrayType(VarDecl* V, QualType T);
    void analyseArraySizeExpr(ArrayType* AT);

    clang::DiagnosticBuilder Diag(clang::SourceLocation Loc, unsigned DiagID) const;
    void pushMode(unsigned DiagID);
    void popMode();

    class ConstModeSetter {
    public:
        ConstModeSetter (FunctionAnalyser& analyser_, unsigned DiagID)
            : analyser(analyser_)
        {
            analyser.pushMode(DiagID);
        }
        ~ConstModeSetter()
        {
            analyser.popMode();
        }
    private:
        FunctionAnalyser& analyser;
    };

    LabelDecl* LookupOrCreateLabel(const std::string& name, clang::SourceLocation loc);

    bool checkAssignee(Expr* expr) const;
    void checkAssignment(Expr* assignee, QualType TLeft);
    void checkDeclAssignment(Decl* decl, Expr* expr);
    void checkArrayDesignators(InitListExpr* expr, int64_t* size);
    void checkEnumCases(const SwitchStmt* SS, const EnumType* ET) const;
    QualType getStructType(QualType T) const;
    QualType getConditionType(const Stmt* C) const;

    // conversions
    QualType UsualUnaryConversions(Expr* expr) const;

    Scope& scope;
    TypeResolver& TR;
    TypeContext& typeContext;
    ExprTypeAnalyser EA;

    clang::DiagnosticsEngine& Diags;

    unsigned errors;
    FunctionDecl* CurrentFunction;
    VarDecl* CurrentVarDecl;
    unsigned constDiagID;
    bool inConstExpr;
    bool usedPublicly;
    bool isInterface;

    typedef std::vector<LabelDecl*> Labels;
    typedef Labels::iterator LabelsIter;
    Labels labels;

    FunctionAnalyser(const FunctionAnalyser&);
    FunctionAnalyser& operator= (const FunctionAnalyser&);
};

}

#endif

