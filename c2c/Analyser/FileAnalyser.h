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

#ifndef ANALYSER_FILE_ANALYSER_H
#define ANALYSER_FILE_ANALYSER_H

#include <memory>
#include <map>
#include <string>
#include "Analyser/FunctionAnalyser.h"
#include "AST/Type.h"
#include "AST/Module.h"

namespace clang {
class DiagnosticsEngine;
}

namespace C2 {

class IdentifierExpr;
class Scope;
class TypeDecl;
class VarDecl;
class FunctionDecl;
class StructTypeDecl;
class ArrayValueDecl;
class AST;
class TypeResolver;

class FileAnalyser {
public:
    FileAnalyser(const Modules& modules, clang::DiagnosticsEngine& Diags_,
                 AST& ast_, bool verbose);
    ~FileAnalyser() {}

    void printAST(bool printInterface) const;

    // call in this order
    void checkImports();
    unsigned resolveTypes();
    unsigned resolveTypeCanonicals();
    unsigned resolveStructMembers();
    unsigned resolveVars();
    unsigned checkArrayValues();
    unsigned checkVarInits();
    unsigned resolveEnumConstants();
    unsigned checkFunctionProtos();
    unsigned checkFunctionBodies();
    void checkDeclsForUsed();

private:
    unsigned checkTypeDecl(TypeDecl* D);
    unsigned checkStructTypeDecl(StructTypeDecl* D);
    unsigned resolveVarDecl(VarDecl* D);
    unsigned resolveFunctionDecl(FunctionDecl* D);
    unsigned checkArrayValue(ArrayValueDecl* D);
    typedef std::map<const std::string, const Decl*> Names;
    void analyseStructNames(const StructTypeDecl* S, Names& names, bool isStruct);
    void checkVarDeclAttributes(VarDecl* D);
    void checkAttributes(Decl* D);
    void checkStructMembersForUsed(const StructTypeDecl* S);

    AST& ast;
    std::auto_ptr<Scope> globals;
    std::auto_ptr<TypeResolver> TR;
    clang::DiagnosticsEngine& Diags;
    FunctionAnalyser functionAnalyser;
    bool verbose;

    FileAnalyser(const FileAnalyser&);
    FileAnalyser& operator= (const FileAnalyser&);
};

}

#endif

