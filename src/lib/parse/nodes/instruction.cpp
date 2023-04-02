// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "visitor.hpp"

#include <exception.hpp>

using namespace WasmVM;

void InstrVisitor::Syntax::operator()(WasmVM::Syntax::PlainInstr& plain){
    std::visit(overloaded {
        [&](auto&& instr){
            body.emplace_back(instr);
        }
    }, plain);
}
void InstrVisitor::Syntax::operator()(WasmVM::Syntax::FoldedInstr& folded){
    for(WasmVM::Syntax::Instr instr : folded.instrs){
        std::visit(InstrVisitor::Syntax(body), instr);
    }
}

std::optional<Syntax::FoldedInstr> Syntax::FoldedInstr::get(TokenIter& begin, TokenHolder& holder){
    std::list<TokenType>::iterator it = begin;
    auto syntax = Parse::Rule<
        Token::ParenL,
        Syntax::PlainInstr, Parse::Repeat<Syntax::FoldedInstr>, 
        Token::ParenR
    >::get(it, holder);

    if(syntax){
        auto rule = syntax.value();
        Syntax::FoldedInstr instr;
        auto plain = std::get<1>(rule);
        for(auto folded : std::get<2>(rule)){
            instr.instrs.splice(instr.instrs.end(), folded.instrs);
        }
        instr.instrs.emplace_back(plain);
        begin = it;
        return instr;
    }
    return std::nullopt;
}