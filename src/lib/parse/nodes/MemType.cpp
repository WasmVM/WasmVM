// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "../syntax.hpp"

using namespace WasmVM;

std::optional<Parse::MemType> Parse::MemType::get(TokenIter& begin, TokenHolder& holder){

    std::list<TokenType>::iterator it = begin;

    auto syntax = Syntax::Limits::get(it, holder);

    if(syntax){
        Parse::MemType mem_type;
        auto rule = syntax.value();
        // limits
        mem_type.min = std::get<0>(rule).unpack<u32_t>();
        auto max_value = std::get<1>(rule);
        if(max_value){
            mem_type.max = max_value->unpack<u32_t>();
            if(mem_type.min > mem_type.max){
                throw Exception::invalid_limit(std::get<0>(rule).location, ": min can't be greater than max");
            }
        }
        begin = it;
        return mem_type;
    }

    return std::nullopt;
}