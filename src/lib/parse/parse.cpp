// Copyright 2022 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <WasmVM.hpp>
#include <parse/WatLexer.h>
#include <parse/WatParser.h>
#include "ErrorListener.hpp"
#include "Visitor.hpp"

using namespace WasmVM;

WasmModule WasmVM::module_parse(std::istream& is){
    antlr4::ANTLRInputStream stream(is);
    WatLexer lexer(&stream);
    WasmVM::LexerErrorListener lexerErrorListener;
    WasmVM::ParserErrorListener parserErrorListener;
    antlr4::CommonTokenStream tokens(&lexer);
    WasmVM::WatParser parser(&tokens);

    lexer.removeErrorListeners();
    lexer.addErrorListener(&lexerErrorListener);
    parser.removeErrorListeners();
    parser.addErrorListener(&parserErrorListener);

    WasmVM::Visitor visitor;
    return visitor.visit(parser.module());
}
