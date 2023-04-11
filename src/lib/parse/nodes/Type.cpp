// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "../nodes.hpp"
#include "visitor.hpp"

#include <string>
#include <Util.hpp>

using namespace WasmVM;

void ModuleVisitor::operator()(Parse::Type& type){
    // id
    if(!type.id.empty()){
        if(typeid_map.contains(type.id)){
            throw Exception::duplicated_identifier(type.location, std::string(" : type ") + type.id);
        }
        typeid_map[type.id] = module.types.size();
    }
    // functype
    FuncType& functype = module.types.emplace_back();
    functype.params = type.func.params;
    functype.results = type.func.results;
    // id map
    paramid_maps.emplace_back(type.func.id_map);
}

std::optional<Parse::Type> Parse::Type::get(TokenIter& begin, TokenHolder& holder){
    std::list<TokenType>::iterator it = begin;

    auto syntax = Parse::Rule<Token::ParenL,
        Token::Keyword<"type">, Parse::Optional<Token::Id>,
        Token::ParenL, Token::Keyword<"func">, Parse::FuncType, Token::ParenR,
        Token::ParenR
    >::get(it, holder);

    if(syntax){
        Parse::Type type;
        auto rule = syntax.value();
        auto id = std::get<2>(rule);
        type.id = id ? id->value : "";
        type.location = id->location;
        type.func = std::get<5>(rule);
        begin = it;
        return type;
    }

    return std::nullopt;
}

index_t Parse::Type::index(Parse::TypeUse& typeuse, WasmModule& module, std::map<std::string, index_t>& typeid_map, std::vector<std::map<std::string, index_t>>& paramid_maps){
    index_t idx = typeuse.index ? std::visit(Parse::Index::Visitor(typeid_map, false), typeuse.index.value()) : index_npos;

    WasmVM::FuncType functype;
    std::map<std::string, index_t> paramid_map;
    if(idx != index_npos){
        if(idx >= module.types.size()){
            throw Exception::index_out_of_range(typeuse.location, " : type not found");
        }
        if(typeuse.functype.params.empty() && typeuse.functype.results.empty()){
            paramid_maps.emplace_back(paramid_maps[idx]);
            return idx;
        }
        functype = module.types[idx];
        paramid_map = paramid_maps[idx];
    }

    size_t param_count = functype.params.size();
    for(auto id_pair : typeuse.functype.id_map){
        if(paramid_map.contains(id_pair.first)){
            throw Exception::duplicated_identifier(typeuse.location, std::string(" : func param ") + id_pair.first);
        }
        paramid_map[id_pair.first] = id_pair.second + param_count;
    }
    functype.params.insert(functype.params.end(), typeuse.functype.params.begin(), typeuse.functype.params.end());
    functype.results.insert(functype.results.end(), typeuse.functype.results.begin(), typeuse.functype.results.end());
    paramid_maps.emplace_back(paramid_map);
    idx = module.types.size();
    module.types.emplace_back(functype);
    return idx;
}
