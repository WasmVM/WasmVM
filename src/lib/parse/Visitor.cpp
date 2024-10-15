// Copyright 2024 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "Visitor.hpp"
#include <WasmVM.hpp>

#include <algorithm>

using namespace WasmVM;

WasmModule Visitor::visit(WatParser::ModuleContext *ctx){
    visitModule(ctx);
    // TODO: post process
    return module;
}

std::any Visitor::visitModule(WatParser::ModuleContext *ctx){
    visitChildren(ctx);
    // types
    std::transform(types.begin(), types.end(), std::back_inserter(module.types), [](auto& type_pair){
        return type_pair.first;
    });
    return module;
}

std::any Visitor::visitModulefield(WatParser::ModulefieldContext *ctx){
    return visitChildren(ctx);
}

std::any Visitor::visitTypesection(WatParser::TypesectionContext *ctx){
    if(ctx->Id() != nullptr){
        type_map[ctx->Id()->getText()] = types.size();
    }
    std::pair<FuncType, std::map<std::string, index_t>> functype = std::any_cast<std::pair<FuncType, std::map<std::string, index_t>>>(visitFunctype(ctx->functype()));
    return types.emplace_back(functype);
}

std::any Visitor::visitValtype(WatParser::ValtypeContext *ctx){
    return visitChildren(ctx);
}

std::any Visitor::visitFunctype(WatParser::FunctypeContext *ctx){
    std::pair<FuncType, std::map<std::string, index_t>> functype;
    // Param
    for(auto param_tree : ctx->param()){
        std::variant<std::pair<std::string, ValueType>, std::vector<ValueType>> param = std::any_cast<std::variant<std::pair<std::string, ValueType>, std::vector<ValueType>>>(visitParam(param_tree));
        std::visit(overloaded {
            [&](std::pair<std::string, ValueType>& param_pair){
                functype.second[param_pair.first] = functype.first.params.size();
                functype.first.params.emplace_back(param_pair.second);
            },
            [&](std::vector<ValueType>& params){
                functype.first.params.insert(functype.first.params.end(), params.begin(), params.end());
            }
        }, param);
    }
    // Result
    return functype;
}
std::any Visitor::visitParam(WatParser::ParamContext *ctx){
    std::variant<std::pair<std::string, ValueType>, std::vector<ValueType>> param;
    if(ctx->Id() != nullptr){
        std::string id = ctx->Id()->getText();
        ValueType valuetype = std::any_cast<ValueType>(visitValtype(ctx->valtype(0)));
        param = std::pair<std::string, ValueType> {id, valuetype};
    }else{
        std::vector<ValueType>& valuetypes = param.emplace<std::vector<ValueType>>();
        for(auto valuetype : ctx->valtype()){
            valuetypes.emplace_back(std::any_cast<ValueType>(visitValtype(valuetype)));
        }
    }
    return param;
}
std::any Visitor::visitResult(WatParser::ResultContext *ctx){
    return visitChildren(ctx);
}