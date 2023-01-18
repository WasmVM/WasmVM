// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "../nodes.hpp"

using namespace WasmVM;

std::optional<Parse::FuncType> Parse::FuncType::get(TokenIter& begin, const TokenIter& end){

    std::list<TokenType>::iterator it = begin;

    auto syntax = Parse::Rule<
        Token::ParenL,
        Token::Keyword,
        Token::ParenR
    >::get(it, end);

    if(syntax){
        begin = it;
        auto rule = syntax.value();
        
        return Parse::FuncType(); // TODO:
    }

    return std::nullopt;
}