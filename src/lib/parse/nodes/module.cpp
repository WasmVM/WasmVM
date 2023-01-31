// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <WasmVM.hpp>
#include "../syntax.hpp"
#include "visitor.hpp"

using namespace WasmVM;

WasmModule WasmVM::module_parse(std::string src){
    std::list<TokenType> tokens = tokenize(src);
    std::list<TokenType>::iterator it = tokens.begin();

    using modulefields = Parse::OneOf<
        Parse::Type,
        Parse::Import
    >;

    WasmModule module;

    auto syntax = Parse::Rule<
        Token::ParenL,
        Token::Keyword<"module">,
        Parse::Optional<Token::Id>,
        Parse::Repeat<modulefields>,
        Token::ParenR
    >::get(it, tokens.end());

    if(syntax){
        auto rule = syntax.value();

        // id
        auto id = std::get<2>(rule);
        if(id){
            module.id = id->value;
        }

        // sections
        ModuleVisitor visitor(module);
        for(auto section : std::get<3>(rule)){
            std::visit(visitor, section);
        }
        
        return module;
    }
    throw Exception::syntax_error();
}