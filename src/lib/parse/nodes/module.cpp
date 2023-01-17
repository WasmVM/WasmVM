// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <WasmVM.hpp>
#include "../parse.hpp"

#include "../print.hpp" // FIXME:

using namespace WasmVM;

WasmModule WasmVM::module_parse(std::string src){
    std::list<TokenType> tokens = tokenize(src);
    std::list<TokenType>::iterator it = tokens.begin();

    auto module_rule = Parse::Rule<
        Token::ParenL,
        Token::Keyword,
        Parse::Optional<Token::Id>,
        Token::ParenR
    >::get(it, tokens.end());`

    if(module_rule){
        auto rule = module_rule.value();
        Printer()(rule);
    }
    
    return WasmModule();
}