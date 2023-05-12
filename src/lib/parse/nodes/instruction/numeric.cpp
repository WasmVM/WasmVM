// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "../visitor.hpp"
#include "numeric.hpp"

using namespace WasmVM;

std::optional<Parse::Instr::NumericInstr> Parse::Instr::NumericInstr::get(TokenIter& begin, TokenHolder& holder){
    std::list<TokenType>::iterator it = begin;
    auto syntax = Parse::OneOf<
        Parse::Numeric_i32,
        Parse::Numeric_i64,
        Parse::Numeric_f32,
        Parse::Numeric_f64
    >::get(it, holder);

    if(syntax){
        begin = it;
        return std::visit(overloaded {
            [&](auto&& instr){
                return instr.numeric;
            }
        }, syntax.value());
    }
    return std::nullopt;
}

template<> void InstrVisitor::Sema::operator()<Parse::Instr::NumericInstr>(Parse::Instr::NumericInstr& node){
    instrs.emplace_back(node.instr);
}