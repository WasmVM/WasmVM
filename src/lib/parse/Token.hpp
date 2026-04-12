// Copyright 2024 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef WASMVM_Parse_Token
#define WASMVM_Parse_Token

#include <string>
#include <cstddef>

namespace WasmVM {
namespace Parse {

enum class TokenType {
    LParen,   // (
    RParen,   // )
    Integer,  // sign? (decimal | 0x hex), underscores allowed
    Float,    // sign? (inf | nan | nan:0x... | decimal | hex float)
    String,   // "..." with escape sequences, raw quoted text
    Id,       // $identifier
    Offset,   // offset=<integer> — text stores numeric suffix only
    Align,    // align=<integer>  — text stores numeric suffix only
    Keyword,  // all bare words: module, func, i32, i32.const, ref.null, etc.
    Eof
};

struct Token {
    TokenType type;
    std::string text;
    size_t line;
    size_t column;
};

} // namespace Parse
} // namespace WasmVM

#endif
