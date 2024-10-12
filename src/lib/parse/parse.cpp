// Copyright 2022 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <WasmVM.hpp>
#include <WatLexer.h>
// #include "WasmParser.h"

using namespace WasmVM;

WasmModule WasmVM::module_parse(std::istream& is){
    antlr4::ANTLRInputStream stream(is);
    WatLexer lexer(&stream);
    auto token = lexer.nextToken();
    std::cout << token->getText() << std::endl;
    // try{
    //     return parser.parse();
    // }catch(ParseError& e){
    //     throw Exception::Parse(e.what(), {e.pos.line, e.pos.column});
    // }
    return WasmModule();
}
