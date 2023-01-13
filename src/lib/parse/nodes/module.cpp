// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <WasmVM.hpp>
#include "../parse.hpp"

#include <iostream> // FIXME:

using namespace WasmVM;

WasmModule WasmVM::module_parse(std::string src){
    std::list<TokenType> tokens = tokenize(src);
    std::list<TokenType>::iterator it = tokens.begin();
    auto module_rule = Parse::Rule<Token::ParenL, Parse::Optional<Token::Keyword>, Token::ParenR>::get(it, tokens.end());
    
    std::cout << std::get<0>(module_rule).has_value() << " " << std::get<1>(module_rule).has_value() << std::endl;
    
    return WasmModule();
}