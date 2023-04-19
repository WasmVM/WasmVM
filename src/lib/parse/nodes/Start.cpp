// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "visitor.hpp"

using namespace WasmVM;

void ModuleVisitor::operator()(Parse::Start& node){
    if(module.start){
        throw Exception::multiple_start(node.location);
    }
    module.start = std::visit(Parse::Index::Visitor(func_indices.id_map), node.funcidx);
}

std::optional<Parse::Start> Parse::Start::get(TokenIter& begin, TokenHolder& holder){
    std::list<TokenType>::iterator it = begin;

    auto syntax = Parse::Rule<
        Token::ParenL, Token::Keyword<"start">, Parse::Index, Token::ParenR
    >::get(it, holder);

    if(syntax){
        auto rule = syntax.value();
        Parse::Start start(std::get<2>(rule));
        start.location = std::get<0>(rule).location;
        begin = it;
        return start;
    }
    return std::nullopt;
}