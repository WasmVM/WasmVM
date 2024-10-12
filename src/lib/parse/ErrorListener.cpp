// Copyright 2024 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "ErrorListener.hpp"

#include <exception.hpp>

void WasmVM::LexerErrorListener::syntaxError(antlr4::Recognizer *recognizer, antlr4::Token* token, size_t line, size_t col, const std::string &msg, std::exception_ptr e){
    throw WasmVM::Exception::Parse("unknown token " + msg.substr(28), {line, col});
}