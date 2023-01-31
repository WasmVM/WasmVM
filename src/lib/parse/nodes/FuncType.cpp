// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "../syntax.hpp"
#include "visitor.hpp"

using namespace WasmVM;

std::optional<Parse::FuncType> Parse::FuncType::get(TokenIter& begin, const TokenIter& end){

    std::list<TokenType>::iterator it = begin;

    auto syntax = Parse::Rule<
        // param
        Parse::Repeat<Parse::Rule<
            Token::ParenL, Token::Keyword<"param">, Parse::Optional<Token::Id>, 
            Parse::Repeat<Parse::ValueType>, 
            Token::ParenR
        >>,
        // result
        Parse::Repeat<Parse::Rule<
            Token::ParenL, Token::Keyword<"result", true>, 
            Parse::Repeat<Parse::ValueType>,
            Token::ParenR
        >>
    >::get(it, end);

    if(syntax){
        Parse::FuncType func_type;
        auto rule = syntax.value();

        // param
        auto params = std::get<0>(rule);
        for(index_t i = 0; i < params.size(); ++i){
            auto param = params[i];
            // location
            Token::Location location = std::get<0>(param).location;
            auto id = std::get<2>(param);
            auto types = std::get<3>(param);
            if(id){
                switch(types.size()){
                    case 0:
                        throw Exception::invalid_functype(location, ": expected one parameter to an identifier");
                    case 1:
                    break;
                    default:
                        throw Exception::invalid_functype(location, ": an identifier can only bind to one parameter");
                }
                func_type.id_map[id->value] = i;
                func_type.params.emplace_back(types.front());
            }else{
                for(Parse::ValueType type : types){
                    func_type.params.emplace_back(type);
                }
            }
        }

        // result
        auto results = std::get<1>(rule);
        for(auto result : results){
            auto types = std::get<2>(result);
            for(Parse::ValueType type : types){
                func_type.results.emplace_back(type);
            }
        }

        begin = it;
        return func_type;
    }

    return std::nullopt;
}