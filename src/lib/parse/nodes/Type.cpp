// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "../nodes.hpp"
#include "visitor.hpp"

using namespace WasmVM;

void ModuleVisitor::operator()(Parse::Type& type){
    // id
    std::string id = std::get<std::string>(type.id);
    if(!id.empty()){
        if(typeid_map.contains(id)){
            throw Exception::duplicated_identifier(type.location, std::string(" : type ") + id);
        }
        typeid_map[id] = module.types.size();
    }
    // functype
    FuncType& functype = module.types.emplace_back();
    functype.params = type.func.params;
    functype.results = type.func.results;
    // id map
    paramid_maps.emplace_back(type.func.id_map);
}

std::optional<Parse::Type> Parse::Type::get(TokenIter& begin, const TokenIter& end){
    std::list<TokenType>::iterator it = begin;

    auto syntax = Parse::Rule<Token::ParenL,
        Token::Keyword<"type">, Parse::Optional<Token::Id>,
        Token::ParenL, Token::Keyword<"func">, Parse::FuncType, Token::ParenR,
        Token::ParenR
    >::get(it, end);

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