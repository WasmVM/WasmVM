// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <WasmVM.hpp>
#include "../syntax.hpp"

#include "../print.hpp" // FIXME:

using namespace WasmVM;

WasmModule WasmVM::module_parse(std::string src){
    std::list<TokenType> tokens = tokenize(src);
    std::list<TokenType>::iterator it = tokens.begin();

    using modulefields = Parse::OneOf<
        Syntax::FuncType
    >;

    // ( module id? modulefields* )
    auto syntax = Parse::Rule<
        Token::ParenL,
        Token::Keyword<"module">,
        Parse::Optional<Token::Id>,
        Parse::Repeat<modulefields>,
        Token::ParenR
    >::get(it, tokens.end());

    if(syntax){
        auto rule = syntax.value();
        WasmModule mod;

        // id
        auto id = std::get<2>(rule);
        if(id){
            mod.id = id->value;
        }

        // sections
        for(auto section : std::get<3>(rule)){
            std::visit(Syntax::Visitor(mod), section);
        }
        
        // Printer()(rule); // FIXME:
        return mod;
    }
    throw Exception::syntax_error();
}