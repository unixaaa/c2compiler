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

#include "Analyser/TargetAnalyser.h"
#include "Analyser/FileAnalyser.h"
#include "AST/Component.h"
#include "AST/Module.h"

using namespace C2;

TargetAnalyser::TargetAnalyser(const Modules& modules_, clang::DiagnosticsEngine& Diags_, Component& C, bool verbose_)
    : modules(modules_)
    , Diags(Diags_)
    , verbose(verbose_)
{
    const ModuleList& mods = C.getModules();
    for (unsigned m=0; m<mods.size(); m++) {
        const Files& files = mods[m]->getFiles();
        for (unsigned f=0; f<files.size(); f++) {
            analysers.push_back(new FileAnalyser(modules, Diags, *files[f], verbose));
        }
    }
}

TargetAnalyser::~TargetAnalyser() {
    for (unsigned i=0; i<analysers.size(); i++) {
        delete analysers[i];
    }
}

unsigned TargetAnalyser::analyse(bool print1, bool print2, bool print3, bool printLib) {
    unsigned errors = 0;
    const size_t count = analysers.size();

    for (unsigned i=0; i<count; i++) {
        analysers[i]->checkImports();
    }
    if (errors) return errors;

    for (unsigned i=0; i<count; i++) {
        errors += analysers[i]->resolveTypes();
    }
    if (errors) return errors;

    for (unsigned i=0; i<count; i++) {
        errors += analysers[i]->resolveTypeCanonicals();
    }
    if (errors) return errors;

    for (unsigned i=0; i<count; i++) {
        errors += analysers[i]->resolveStructMembers();
    }
    if (print1) printASTs(printLib);
    if (errors) return errors;

    for (unsigned i=0; i<count; i++) {
        errors += analysers[i]->resolveVars();
    }
    if (errors) return errors;

    for (unsigned i=0; i<count; i++) {
        errors += analysers[i]->resolveEnumConstants();
    }
    if (errors) return errors;

    for (unsigned i=0; i<count; i++) {
        errors += analysers[i]->checkArrayValues();
    }
    if (errors) return errors;

    for (unsigned i=0; i<count; i++) {
        errors += analysers[i]->checkVarInits();
    }
    if (print2) printASTs(printLib);
    if (errors) return errors;

    for (unsigned i=0; i<count; i++) {
        errors += analysers[i]->checkFunctionProtos();
    }
    if (errors) return errors;

    for (unsigned i=0; i<count; i++) {
        errors += analysers[i]->checkFunctionBodies();
    }

    for (unsigned i=0; i<count; i++) {
        analysers[i]->checkDeclsForUsed();
    }

    if (print3) printASTs(printLib);
    return errors;
}

void TargetAnalyser::printASTs(bool printLib) const {
    for (unsigned i=0; i<analysers.size(); i++) {
        analysers[i]->printAST(printLib);
    }
}

