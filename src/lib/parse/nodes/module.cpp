// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <WasmVM.hpp>
#include "../syntax.hpp"
#include "visitor.hpp"

using namespace WasmVM;

WasmModule WasmVM::module_parse(std::string src){
    std::list<TokenType> tokens = tokenize(src);

    using modulefields = Parse::Repeat<Parse::OneOf<
        Parse::Type,
        Parse::Import,
        Parse::Func,
        Parse::Table
    >>;

    using modulerule = Parse::Rule<
        Token::ParenL, Token::Keyword<"module">, Parse::Optional<Token::Id>, modulefields, Token::ParenR
    >;

    WasmModule module;
    TokenIter it = tokens.begin();
    TokenHolder holder(tokens.begin(), tokens.end());

    auto syntax = Parse::OneOf<modulerule, modulefields>::get(it, holder);

    if(syntax && !holder.has_next(it)){
        auto sections = std::visit(overloaded {
            [&](modulerule& rule){
                // id
                auto id = std::get<2>(rule);
                if(id){
                    module.id = id->value;
                }
                return std::get<3>(rule);
            },
            [&](modulefields& fields){
                return fields;
            }
        }, syntax.value());

        // sections
        ModuleVisitor visitor(module);
        for(auto section : sections){
            std::visit(visitor, section);
        }
        
        return module;
    }
    if(holder.has_next(it)){
        throw holder.error();
    }else{
        throw Exception::unexpected_eof();
    }
}