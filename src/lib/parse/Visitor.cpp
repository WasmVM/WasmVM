// Copyright 2024 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "Visitor.hpp"
#include <WasmVM.hpp>

#include <algorithm>
#include <tuple>

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
        antlr4::Token* token = ctx->Id()->getSymbol();
        std::string id = ctx->Id()->getText();
        if(type_map.contains(id)){
            throw Exception::Parse("duplicated type id '" + id + "'", {token->getLine(), token->getCharPositionInLine()});
        }
        type_map[id] = types.size();
    }
    std::pair<FuncType, std::map<std::string, index_t>> functype = std::any_cast<std::pair<FuncType, std::map<std::string, index_t>>>(visitFunctype(ctx->functype()));
    return types.emplace_back(functype);
}

std::any Visitor::visitValtype(WatParser::ValtypeContext *ctx){
    if(ctx->NumType()){
        std::string numtype = ctx->NumType()->getText();
        if(numtype == "i32"){
            return ValueType(ValueType::i32);
        }else if(numtype == "i64"){
            return ValueType(ValueType::i64);
        }else if(numtype == "f32"){
            return ValueType(ValueType::f32);
        }else{
            return ValueType(ValueType::f64);
        }
    }else{
        std::string reftype = ctx->RefType()->getText();
        if(reftype == "funcref"){
            return ValueType(ValueType::funcref);
        }else{
            return ValueType(ValueType::externref);
        }
    }
}

std::any Visitor::visitFunctype(WatParser::FunctypeContext *ctx){
    std::pair<FuncType, std::map<std::string, index_t>> functype;
    // Param
    for(auto param_tree : ctx->param()){
        std::variant<std::tuple<std::string, ValueType, std::pair<size_t, size_t>>, std::vector<ValueType>> param = std::any_cast<std::variant<std::tuple<std::string, ValueType, std::pair<size_t, size_t>>, std::vector<ValueType>>>(visitParam(param_tree));
        std::visit(overloaded {
            [&](std::tuple<std::string, ValueType, std::pair<size_t, size_t>>& param_pair){
                if(functype.second.contains(std::get<0>(param_pair))){
                    throw Exception::Parse("duplicated param id '" + std::get<0>(param_pair) + "'", std::get<2>(param_pair));
                }
                functype.second[std::get<0>(param_pair)] = functype.first.params.size();
                functype.first.params.emplace_back(std::get<1>(param_pair));
            },
            [&](std::vector<ValueType>& params){
                functype.first.params.insert(functype.first.params.end(), params.begin(), params.end());
            }
        }, param);
    }
    // Result
    for(auto result_tree : ctx->result()){
        auto results = std::any_cast<std::vector<ValueType>>(visitResult(result_tree));
        functype.first.results.insert(functype.first.results.end(), results.begin(), results.end());
    }
    
    return functype;
}
std::any Visitor::visitParam(WatParser::ParamContext *ctx){
    std::variant<std::tuple<std::string, ValueType, std::pair<size_t, size_t>>, std::vector<ValueType>> param;
    if(ctx->Id() != nullptr){
        auto symbol = ctx->Id()->getSymbol();
        std::pair<size_t, size_t> location {symbol->getLine(), symbol->getCharPositionInLine()};
        std::string id = ctx->Id()->getText();
        ValueType valuetype = std::any_cast<ValueType>(visitValtype(ctx->valtype(0)));
        param = std::tuple<std::string, ValueType, std::pair<size_t, size_t>> {id, valuetype, location};
    }else{
        std::vector<ValueType>& valuetypes = param.emplace<std::vector<ValueType>>();
        for(auto valuetype : ctx->valtype()){
            valuetypes.emplace_back(std::any_cast<ValueType>(visitValtype(valuetype)));
        }
    }
    return param;
}
std::any Visitor::visitResult(WatParser::ResultContext *ctx){
    std::vector<ValueType> results;
    for(auto valuetype : ctx->valtype()){
        results.emplace_back(std::any_cast<ValueType>(visitValtype(valuetype)));
    }
    return results;
}