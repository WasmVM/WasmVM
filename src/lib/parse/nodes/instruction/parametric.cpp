// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "../visitor.hpp"
#include "macro.hpp"

using namespace WasmVM;

SemaTemplate(Drop);
SemaTemplate(Select);

std::optional<Parse::Instr::Select> Parse::Instr::Select::get(TokenIter& begin, TokenHolder& holder){
    std::list<TokenType>::iterator it = begin;
    auto syntax = Parse::Rule<
        Token::Keyword<"select">, 
        Parse::Repeat<
            Rule<Token::ParenL, Token::Keyword<"result">, Repeat<ValueType>, Token::ParenR>
        >
    >::get(it, holder);

    if(syntax){
        auto rule = syntax.value();
        Parse::Instr::Select instr;
        for(auto result : std::get<1>(rule)){
            for(auto type : std::get<2>(result)){
                instr.valtypes.emplace_back(type);
            }
        }
        begin = it;
        return instr;
    }
    return std::nullopt;
}
