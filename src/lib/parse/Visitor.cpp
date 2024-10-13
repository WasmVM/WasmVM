// Copyright 2024 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "Visitor.hpp"
#include <WasmVM.hpp>

using namespace WasmVM;

std::any Visitor::visitModule(WatParser::ModuleContext *context){
    visitChildren(context);
    return module;
}

std::any Visitor::visitModulefield(WatParser::ModulefieldContext *context){
    return visitChildren(context);
}

std::any Visitor::visitType(WatParser::TypeContext *context){
    return visitChildren(context);
}

std::any Visitor::visitValtype(WatParser::ValtypeContext *context){
    return visitChildren(context);
}

std::any Visitor::visitParam(WatParser::ParamContext *context){
    return visitChildren(context);
}

std::any Visitor::visitResult(WatParser::ResultContext *context){
    return visitChildren(context);
}

std::any Visitor::visitFunctype(WatParser::FunctypeContext *context){
    return visitChildren(context);
}

std::any Visitor::visitLimits(WatParser::LimitsContext *context){
    return visitChildren(context);
}

std::any Visitor::visitMemtype(WatParser::MemtypeContext *context){
    return visitChildren(context);
}

std::any Visitor::visitTabletype(WatParser::TabletypeContext *context){
    return visitChildren(context);
}

std::any Visitor::visitGlobaltype(WatParser::GlobaltypeContext *context){
    return visitChildren(context);
}