// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "../nodes.hpp"

using namespace WasmVM;

std::optional<Parse::TypeUse> Parse::TypeUse::get(TokenIter& begin, TokenHolder& holder){
    std::list<TokenType>::iterator it = begin;

    auto syntax = Parse::Rule<
        Parse::Optional<
            Parse::Rule<Token::ParenL, Token::Keyword<"type">, Parse::Index, Token::ParenR>
        >,
        Parse::Repeat<
            Parse::Rule<Token::ParenL, Token::Keyword<"param">, Parse::Optional<Token::Id>, Parse::Repeat<Parse::ValueType>, Token::ParenR>
        >,
        Parse::Repeat<
            Parse::Rule<Token::ParenL, Token::Keyword<"result">, Parse::Repeat<Parse::ValueType>, Token::ParenR>
        >
    >::get(it, holder);

    if(syntax){
        Parse::TypeUse typeuse;
        auto rule = syntax.value();
        // index
        auto index_rule = std::get<0>(rule);
        if(index_rule){
            typeuse.index = std::get<2>(index_rule.value());
            typeuse.location = std::get<0>(index_rule.value()).location;
        }else if(std::get<1>(rule).size() > 0){
            typeuse.location = std::get<0>(std::get<1>(rule).front()).location;
        }
        // params
        for(auto param_rule : std::get<1>(rule)){
            auto id = std::get<2>(param_rule);
            auto values = std::get<3>(param_rule);
            if(id){
                switch(values.size()){
                    case 0:
                        throw Exception::invalid_functype(id->location, ": expected one value type to an identifier");
                    case 1:
                        typeuse.functype.id_map[id->value] = typeuse.functype.params.size();
                        typeuse.functype.params.emplace_back(values.front());
                    break;
                    default:
                        throw Exception::invalid_functype(id->location, ": an identifier can only bind to one parameter");
                }
            }else{
                for(Parse::ValueType param : values){
                    typeuse.functype.params.emplace_back(param);
                }
            }
        }
        // results
        for(auto result_rule : std::get<2>(rule)){
            for(Parse::ValueType result : std::get<2>(result_rule)){
                typeuse.functype.results.emplace_back(result);
            }
        }
        begin = it;
        return typeuse;
    }

    return std::nullopt;
}