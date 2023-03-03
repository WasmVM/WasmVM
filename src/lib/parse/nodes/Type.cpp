// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "../nodes.hpp"
#include "visitor.hpp"

#include <string>
#include <Util.hpp>

using namespace WasmVM;

Parse::Type::Type(Parse::TypeUse& typeuse){
    // index
    auto index = std::get<0>(typeuse);
    if(index){
        auto typeidx = std::get<2>(index.value());
        std::visit(overloaded {
            [&](Token::Number token){
                id.emplace<index_t>(token.unpack<index_t>());
                location = token.location;
            },
            [&](Token::Id token){
                id.emplace<std::string>(token.value);
                location = token.location;
            }
        }, typeidx);
    }

    // param
    auto params = std::get<1>(typeuse);
    for(index_t i = 0; i < params.size(); ++i){
        // location
        auto param = params[i];
        Token::Location location = std::get<0>(param).location;
        auto idx = std::get<2>(param);
        auto types = std::get<3>(param);
        if(idx){
            switch(types.size()){
                case 0:
                    throw Exception::invalid_functype(location, ": expected one parameter to an identifier");
                case 1:
                break;
                default:
                    throw Exception::invalid_functype(location, ": an identifier can only bind to one parameter");
            }
            if(func.id_map.contains(idx->value)){
                throw Exception::duplicated_identifier(idx->location, std::string(": param ") + idx->value);
            }
            func.id_map[idx->value] = i;
            func.params.emplace_back(types.front());
        }else{
            for(Parse::ValueType type : types){
                func.params.emplace_back(type);
            }
        }
    }

    // result
    for(auto result : std::get<2>(typeuse)){
        auto types = std::get<2>(result);
        for(Parse::ValueType type : types){
            func.results.emplace_back(type);
        }
    }
}

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

index_t Parse::Type::index(WasmModule& module, std::map<std::string, index_t>& typeid_map, std::vector<std::map<std::string, index_t>>& paramid_maps){
    index_t idx = std::visit(overloaded {
        [&](std::string ind){
            if(ind.empty()){
                return index_npos;
            }else if(typeid_map.contains(ind)){
                return typeid_map[ind];
            }else{
                throw Exception::unknown_identifier(location, std::string(" type '") + ind + "' not found");
            }
        },
        [&](index_t ind){
            return ind;
        }
    }, id);

    WasmVM::FuncType functype;
    std::map<std::string, index_t> paramid_map;
    if(idx != index_npos){
        if(idx >= module.types.size()){
            throw Exception::index_out_of_range(location, " : type not found");
        }
        if(func.params.empty() && func.results.empty()){
            paramid_maps.emplace_back(paramid_maps[idx]);
            return idx;
        }
        functype = module.types[idx];
        paramid_map = paramid_maps[idx];
    }

    size_t param_count = functype.params.size();
    for(auto id_pair : func.id_map){
        if(paramid_map.contains(id_pair.first)){
            throw Exception::duplicated_identifier(location, std::string(" : func param ") + id_pair.first);
        }
        paramid_map[id_pair.first] = id_pair.second + param_count;
    }
    functype.params.insert(functype.params.end(), func.params.begin(), func.params.end());
    functype.results.insert(functype.results.end(), func.results.begin(), func.results.end());
    paramid_maps.emplace_back(paramid_map);
    idx = module.types.size();
    module.types.emplace_back(functype);
    return idx;
}