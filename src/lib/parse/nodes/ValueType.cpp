// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "../nodes.hpp"

using namespace WasmVM;

std::optional<Parse::ValueType> Parse::ValueType::get(TokenIter& begin, const TokenIter& end){

    std::list<TokenType>::iterator it = begin;

    auto syntax = Token::Keyword::get(it, end);

    if(syntax){
        auto keyword = syntax.value();
        if(keyword.value == "i32"){
            begin = it;
            return ValueType(WasmVM::ValueType::i32);
        }else if(keyword.value == "i64"){
            begin = it;
            return ValueType(WasmVM::ValueType::i64);
        }else if(keyword.value == "f32"){
            begin = it;
            return ValueType(WasmVM::ValueType::f32);
        }else if(keyword.value == "f64"){
            begin = it;
            return ValueType(WasmVM::ValueType::f64);
        }else if(keyword.value == "func" || keyword.value == "funcref"){
            begin = it;
            return ValueType(WasmVM::ValueType::funcref);
        }else if(keyword.value == "extern" || keyword.value == "externref"){
            begin = it;
            return ValueType(WasmVM::ValueType::externref);
        }
    }

    return std::nullopt;
}