// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <WasmVM.hpp>
#include "../parse.hpp"
#include "../nodes.hpp"

#include "../print.hpp" // FIXME:

using namespace WasmVM;

WasmModule WasmVM::module_parse(std::string src){
    std::list<TokenType> tokens = tokenize(src);
    std::list<TokenType>::iterator it = tokens.begin();

    // ( type id? functype )
    using Type = Parse::Rule<
        Token::ParenL, Token::Keyword<"type", true>, Parse::Optional<Token::Id>, Parse::FuncType, Token::ParenR
    >;

    using modulefields = Parse::OneOf<
        Type
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
        auto id = std::get<2>(rule);
        if(id){
            mod.id = id->value;
        }
        
        // Printer()(rule); // FIXME:
        return mod;
    }
    throw Exception::syntax_error();
}