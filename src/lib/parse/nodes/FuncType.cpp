// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "../nodes.hpp"

using namespace WasmVM;

std::optional<Parse::FuncType> Parse::FuncType::get(TokenIter& begin, const TokenIter& end){

    std::list<TokenType>::iterator it = begin;

    auto syntax = Parse::Rule<
        Token::ParenL, Token::Keyword,
        Parse::Repeat<Parse::Rule<
            Token::ParenL, Token::Keyword, Parse::Optional<Token::Id>, 
            Parse::Repeat<Parse::ValueType>, 
            Token::ParenR
        >>,
        Token::ParenR
    >::get(it, end);

    if(syntax){
        Parse::FuncType func_type;
        auto rule = syntax.value();

        // func keyword
        Token::Keyword type_keyword = std::get<1>(rule);
        if(type_keyword.value != "func"){
            return std::nullopt;
        }

        // param
        auto params = std::get<2>(rule);
        auto param_it = params.begin();
        for(; param_it != params.end(); param_it = std::next(param_it)){
            auto param = *param_it;
            // location
            Token::Location location = std::get<0>(param).location;
            std::string param_keyword = std::get<1>(param).value;
            if(param_keyword != "param"){
                if(param_keyword == "result"){
                    break;
                }
                return std::nullopt;
            }
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
                func_type.params.emplace_back(id.value().value, types.front().type);
            }else{
                for(Parse::ValueType type : types){
                    func_type.params.emplace_back("", type.type);
                }
            }
        }

        // result
        for(; param_it != params.end(); param_it = std::next(param_it)){
            auto result = *param_it;
            Token::Location location = std::get<0>(result).location;
            std::string result_keyword = std::get<1>(result).value;
            if(result_keyword != "result"){
                if(result_keyword == "param"){
                    throw Exception::invalid_functype(location, ": params should be defined before results");
                }
                return std::nullopt;
            }
            if(std::get<2>(result)){
                throw Exception::invalid_functype(location, ": cannot bind identifier to results");
            }
            auto types = std::get<3>(result);
            for(Parse::ValueType type : types){
                func_type.results.emplace_back(type.type);
            }
        }

        begin = it;
        return func_type;
    }

    return std::nullopt;
}