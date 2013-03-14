/* Copyright 2013 Bas van den Berg
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

#ifndef FUNCTION_BODY_ANALYSER_H
#define FUNCTION_BODY_ANALYSER_H

#include "ASTVisitor.h"
#include "Package.h"
#include "Scope.h"

#define MAX_SCOPE_DEPTH 15

namespace clang {
class DiagnosticsEngine;
}

namespace C2 {

class Type;
class GlobalScope;
class Decl;
class Stmt;
class Expr;
class IdentifierExpr;

class FunctionBodyAnalyser : public ASTVisitor {
public:
    FunctionBodyAnalyser(GlobalScope& scope_, clang::DiagnosticsEngine& Diags_);
    virtual ~FunctionBodyAnalyser();

    virtual bool handle(Decl* decl);
    unsigned int getErrors() const { return errors; }
private:
    void EnterScope(unsigned int flags);
    void ExitScope();

    void analyseStmt(Stmt* stmt, bool haveScope = false);
    void analyseCompoundStmt(Stmt* stmt);
    void analyseIfStmt(Stmt* stmt);
    void analyseWhileStmt(Stmt* stmt);
    void analyseDoStmt(Stmt* stmt);
    void analyseForStmt(Stmt* stmt);
    void analyseSwitchStmt(Stmt* stmt);
    void analyseBreakStmt(Stmt* S);
    void analyseContinueStmt(Stmt* S);
    void analyseCaseStmt(Stmt* stmt);
    void analyseDefaultStmt(Stmt* stmt);
    void analyseReturnStmt(Stmt* stmt);

    void analyseStmtExpr(Stmt* stmt);
    void analyseExpr(Expr* expr);
    void analyseDeclExpr(Expr* expr);
    void analyseBinOpExpr(Expr* expr);
    void analyseUnaryOpExpr(Expr* expr);
    void analyseSizeofExpr(Expr* expr);
    void analyseArraySubscript(Expr* expr);
    void analyseCall(Expr* expr);
    void analyseIdentifier(Expr* expr);

    GlobalScope& globalScope;
    Scope scopes[MAX_SCOPE_DEPTH];
    unsigned scopeIndex;    // first free scope (= count of scopes)
    Scope* curScope;
    //const Pkgs& pkgs;
    clang::DiagnosticsEngine& Diags;
    unsigned int errors;

    FunctionBodyAnalyser(const FunctionBodyAnalyser&);
    FunctionBodyAnalyser& operator= (const FunctionBodyAnalyser&);
};

}

#endif
