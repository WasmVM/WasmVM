// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "../visitor.hpp"
#include "macro.hpp"

using namespace WasmVM;

SemaTemplate(Ref_is_null)
SemaTemplate(Ref_null)

template<> void InstrVisitor::Sema::operator()<Parse::Instr::Ref_func>(Parse::Instr::Ref_func& instr){
    index_t index = std::visit(Parse::Index::Visitor(module.func_indices.id_map), instr.index);
    instrs.emplace_back<WasmVM::Instr::Ref_func>(index);
}

std::optional<Parse::Instr::Ref_null> Parse::Instr::Ref_null::get(TokenIter& begin, TokenHolder& holder){
    std::list<TokenType>::iterator it = begin;
    auto syntax = Parse::Rule<
        Token::Keyword<"ref.null">, Parse::OneOf<Token::Keyword<"func">, Token::Keyword<"extern">>
    >::get(it, holder);

    if(syntax){
        auto rule = syntax.value();
        begin = it;
        return Parse::Instr::Ref_null(std::visit(overloaded {
            [](Token::Keyword<"func">){
                return WasmVM::RefType::funcref;
            },
            [](Token::Keyword<"extern">){
                return WasmVM::RefType::externref;
            },
        }, std::get<1>(rule)));
    }
    return std::nullopt;
}
