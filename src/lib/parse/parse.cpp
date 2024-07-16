// Copyright 2022 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <WasmVM.hpp>
#include "Lexer.hpp"
#include "Parser.hpp"

using namespace WasmVM;

WasmModule WasmVM::module_parse(std::istream& stream){
    Lexer lexer("", stream);
    Parser parser(lexer);
    try{
        return parser.parse();
    }catch(ParseError& e){
        throw Exception::Parse(e.what(), {e.pos.line, e.pos.column});
    }
}
