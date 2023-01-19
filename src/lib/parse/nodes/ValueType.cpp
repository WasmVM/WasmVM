// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "../nodes.hpp"

#include <Util.hpp>

using namespace WasmVM;

std::optional<Parse::ValueType> Parse::ValueType::get(TokenIter& begin, const TokenIter& end){

    std::list<TokenType>::iterator it = begin;

    auto syntax = Parse::OneOf<
        Token::Keyword<"i32">,
        Token::Keyword<"i64">,
        Token::Keyword<"f32">,
        Token::Keyword<"f64">,
        Token::Keyword<"funcref">,
        Token::Keyword<"externref">,
        Token::Keyword<"func">,
        Token::Keyword<"extern">
    >::get(it, end);

    if(syntax){
        begin = it;
        return std::visit(overloaded {
            [](Token::Keyword<"i32">&){
                return WasmVM::ValueType::i32;
            },
            [](Token::Keyword<"i64">&){
                return WasmVM::ValueType::i64;
            },
            [](Token::Keyword<"f32">&){
                return WasmVM::ValueType::f32;
            },
            [](Token::Keyword<"f64">&){
                return WasmVM::ValueType::f64;
            },
            [](Token::Keyword<"func">&){
                return WasmVM::ValueType::funcref;
            },
            [](Token::Keyword<"funcref">&){
                return WasmVM::ValueType::funcref;
            },
            [](Token::Keyword<"extern">&){
                return WasmVM::ValueType::externref;
            },
            [](Token::Keyword<"externref">&){
                return WasmVM::ValueType::externref;
            },
        }, syntax.value());
    }

    return std::nullopt;
}