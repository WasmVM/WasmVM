// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <WasmVM.hpp>
#include "../syntax.hpp"
#include "visitor.hpp"

using namespace WasmVM;

WasmModule WasmVM::module_parse(std::string src){
    std::list<TokenType> tokens = tokenize(src);

    using modulefields = Parse::OneOf<
        Parse::Type,
        Parse::Import,
        Parse::Func
    >;

    WasmModule module;
    TokenIter it = tokens.begin();
    TokenHolder holder(tokens.begin(), tokens.end());

    auto syntax = Parse::Rule<
        Parse::Optional<Parse::Rule<Token::ParenL, Token::Keyword<"module">>>,
        Parse::Optional<Token::Id>,
        Parse::Repeat<modulefields>,
        Parse::Optional<Token::ParenR>
    >::get(it, holder);

    if(syntax){
        auto rule = syntax.value();

        // parenthesis
        auto parenL = std::get<0>(rule);
        auto parenR = std::get<3>(rule);
        if((!(bool)parenL) != (!(bool)parenR)){
            throw Exception::brackets_not_close(
                parenL ? std::get<0>(parenL.value()).location : parenR->location,
                " in module"
            );
        }

        // id
        auto id = std::get<1>(rule);
        if(id){
            module.id = id->value;
        }

        // sections
        ModuleVisitor visitor(module);
        for(auto section : std::get<2>(rule)){
            std::visit(visitor, section);
        }
        
        return module;
    }
    throw holder.error();
}