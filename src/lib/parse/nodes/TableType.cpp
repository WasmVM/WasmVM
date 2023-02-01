// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "../syntax.hpp"

using namespace WasmVM;

std::optional<Parse::TableType> Parse::TableType::get(TokenIter& begin, const TokenIter& end){

    std::list<TokenType>::iterator it = begin;

    auto syntax = Parse::Rule<Syntax::Limits, Syntax::RefType>::get(it, end);

    if(syntax){
        Parse::TableType table_type;
        auto rule = syntax.value();
        // limits
        auto limits = std::get<0>(rule);
        table_type.limits.min = std::get<0>(limits).unpack<u32_t>();
        auto max_value = std::get<1>(limits);
        if(max_value){
            table_type.limits.max = max_value->unpack<u32_t>();
        }
        // reftype
        std::visit(overloaded {
            [&](Token::Keyword<"funcref">){
                table_type.reftype = WasmVM::RefType::funcref;
            },
            [&](Token::Keyword<"externref">){
                table_type.reftype = WasmVM::RefType::externref;
            }
        }, std::get<1>(rule));
        begin = it;
        return table_type;
    }

    return std::nullopt;
}