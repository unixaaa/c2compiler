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

#include <stdio.h>
#include <assert.h>

#include "AST/Module.h"
#include "AST/Decl.h"
#include "Utils/StringBuilder.h"
#include "Utils/color.h"

using namespace C2;

static std::string empty = "";

Module::Module(const std::string& name_, bool isExternal_, bool isCLib_)
    : name(name_)
    , m_isExternal(isExternal_)
    , m_isCLib(isCLib_)
    , m_isExported(false)
{}

Module::~Module() {
    for (unsigned i=0; i<files.size(); i++) {
        delete files[i];
    }
}

const std::string& Module::getCName() const {
    if (m_isCLib) return empty;
    return name;
}

void Module::addSymbol(Decl* decl) {
    symbols[decl->getName()] = decl;
    decl->setModule(this);
}

Decl* Module::findSymbol(const std::string& name_) const {
    SymbolsConstIter iter = symbols.find(name_);
    if (iter == symbols.end()) return 0;
    else return iter->second;
}

void Module::addAttributes(AttrMap& am) {
    declAttrs.insert(am.begin(), am.end());
}

void Module::printSymbols(StringBuilder& out) const {
    out.indent(2);
    out << "module " << name << '\n';;
    for (SymbolsConstIter iter = symbols.begin(); iter != symbols.end(); ++iter) {
        const Decl* D = iter->second;
        out.indent(4);
        out << D->getName() << "    ";// << '\n';
        out.setColor(COL_ATTRIBUTES);
        switch (D->getKind()) {
        case DECL_FUNC:
            out << "function";
            break;
        case DECL_VAR:
            out << "variable";
            break;
        case DECL_ALIASTYPE:
        case DECL_STRUCTTYPE:
        case DECL_ENUMTYPE:
        case DECL_FUNCTIONTYPE:
            out << "type";
            break;
        case DECL_ENUMVALUE:
            out << "constant";
            break;
        case DECL_ARRAYVALUE:
        case DECL_IMPORT:
        case DECL_LABEL:
            // never symbol
            break;
        }
        if (D->isPublic()) out << " public";
        if (D->isExported()) out << " exported";
        out.setColor(COL_NORM);
        out << '\n';
    }
}

void Module::print(StringBuilder& output) const {
    output << name;
    if (m_isCLib || m_isExported || m_isExternal) {
        output.setColor(COL_ATTRIBUTES);
        output << "  ";
        if (m_isCLib) output << " clib";
        if (m_isExternal) output << " external";
        if (m_isExported) output << " exported";
        output.setColor(ANSI_NORMAL);
    }
}

void Module::printFiles(StringBuilder& out) const {
    out << "  " << name << "\n";
    for (unsigned i=0; i<files.size(); i++) {
        out << "    " << files[i]->getFileName() << '\n';
    }
}

const AttrList& Module::getAttributes(const Decl* d) const {
    assert(d->hasAttributes());
    AttrMapConstIter iter = declAttrs.find(d);
    assert(iter != declAttrs.end() && "called before Decl is linked to module!");
    return iter->second;
}

