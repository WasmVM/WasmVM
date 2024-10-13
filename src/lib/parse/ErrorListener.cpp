// Copyright 2024 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "ErrorListener.hpp"

#include <exception.hpp>

void WasmVM::LexerErrorListener::syntaxError(antlr4::Recognizer *, antlr4::Token*, size_t line, size_t col, const std::string &msg, std::exception_ptr){
    throw WasmVM::Exception::Parse("unknown token " + msg.substr(28), {line, col});
}

void WasmVM::ParserErrorListener::syntaxError(antlr4::Recognizer *, antlr4::Token*, size_t line, size_t col, const std::string &msg, std::exception_ptr){
    throw WasmVM::Exception::Parse(msg, {line, col});
}