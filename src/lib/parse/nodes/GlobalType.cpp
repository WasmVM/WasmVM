// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "../syntax.hpp"
#include "visitor.hpp"

using namespace WasmVM;

std::optional<Parse::GlobalType> Parse::GlobalType::get(TokenIter& begin, const TokenIter& end){

    std::list<TokenType>::iterator it = begin;

    auto syntax = Parse::OneOf<
        Parse::ValueType,
        Parse::Rule<Token::ParenL, Token::Keyword<"mut">, Parse::ValueType, Token::ParenR>
    >::get(it, end);

    if(syntax){
        Parse::GlobalType global_type;
        auto rule = syntax.value();
        std::visit(overloaded {
            [&](Parse::ValueType val){
                global_type.mut = WasmVM::GlobalType::Mut::constant;
                global_type.type = val;
            },
            [&](auto mut_val){
                global_type.mut = WasmVM::GlobalType::Mut::variable;
                global_type.type = std::get<2>(mut_val);
            }
        }, rule);
        begin = it;
        return global_type;
    }

    return std::nullopt;
}