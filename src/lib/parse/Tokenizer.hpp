// Copyright 2024 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef WASMVM_Parse_Tokenizer
#define WASMVM_Parse_Tokenizer

#include "Token.hpp"
#include <exception.hpp>
#include <istream>
#include <string>
#include <utility>

namespace WasmVM {
namespace Parse {

class Tokenizer {
public:
    explicit Tokenizer(std::istream& stream);

    // Peek at current token without consuming.
    const Token& peek() const;

    // Peek at the token after the current one (one position ahead).
    const Token& peek2();

    // Consume and return current token.
    Token consume();

    // Consume and return if type matches; otherwise throw Parse exception.
    Token expect(TokenType type, const std::string& hint = "");

    // Consume and return if current token is Keyword with text==kw; otherwise throw.
    Token expect_keyword(const std::string& kw);

    // True if current token is Keyword with text==kw (does not consume).
    bool peek_keyword(const std::string& kw) const;

    // True if end of stream.
    bool at_eof() const;

    // Location of the current token.
    std::pair<size_t, size_t> location() const;

private:
    std::string source_;
    size_t pos_;
    size_t line_;
    size_t col_;

    // Two-token lookahead buffer: current_ and next_
    Token current_;
    Token next_;
    bool next_valid_;  // whether next_ has been pre-scanned

    void advance_current();       // refill current_ from next_ or scan
    Token scan_next();            // scan one token from pos_/line_/col_

    void skip_whitespace_and_comments();
    Token scan_string();
    Token scan_id();
    Token scan_number(size_t start_line, size_t start_col);
    Token scan_keyword(size_t start_line, size_t start_col);

    char peek_char() const;
    char consume_char();
    bool starts_with(const std::string& prefix) const;
};

} // namespace Parse
} // namespace WasmVM

#endif
