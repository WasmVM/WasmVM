// Copyright 2024 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "Tokenizer.hpp"

#include <iterator>
#include <sstream>
#include <algorithm>

using namespace WasmVM;
using namespace WasmVM::Parse;

// ── helpers ──────────────────────────────────────────────────────────────────

static bool is_digit(char c)     { return c >= '0' && c <= '9'; }
static bool is_hexdigit(char c)  { return is_digit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'); }

// Characters that may appear in a WAT keyword (bare word).
// Anything in: a-z A-Z 0-9 ! # $ % & ' * + - . / : < = > ? @ \ ^ _ ` | ~
// and dot is included so "i32.const" scans as one token.
static bool is_keyword_char(char c) {
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))
        return true;
    switch (c) {
        case '!': case '#': case '$': case '%': case '&': case '\'':
        case '*': case '+': case '-': case '.': case '/': case ':':
        case '<': case '>': case '?': case '@': case '\\':
        case '^': case '_': case '`': case '|': case '~':
            return true;
        default:
            return false;
    }
}

// True when a sign character (+/-) is followed by something that makes a number.
static bool sign_starts_number(const std::string& src, size_t pos) {
    // pos points to the char AFTER the sign
    if (pos >= src.size()) return false;
    char c = src[pos];
    if (is_digit(c)) return true;
    if (c == '0' && pos + 1 < src.size() && src[pos + 1] == 'x') return true;
    // +inf / -inf / +nan / -nan
    if (src.compare(pos, 3, "inf") == 0) return true;
    if (src.compare(pos, 3, "nan") == 0) return true;
    return false;
}

// ── Tokenizer ─────────────────────────────────────────────────────────────────

Tokenizer::Tokenizer(std::istream& stream)
    : pos_(0), line_(1), col_(0), next_valid_(false)
{
    std::ostringstream ss;
    ss << stream.rdbuf();
    source_ = ss.str();
    // pre-scan the first token into current_
    current_ = scan_next();
}

const Token& Tokenizer::peek() const {
    return current_;
}

const Token& Tokenizer::peek2() {
    if (!next_valid_) {
        next_ = scan_next();
        next_valid_ = true;
    }
    return next_;
}

Token Tokenizer::consume() {
    Token t = current_;
    if (next_valid_) {
        current_ = next_;
        next_valid_ = false;
    } else {
        current_ = scan_next();
    }
    return t;
}

Token Tokenizer::expect(TokenType type, const std::string& hint) {
    if (current_.type != type) {
        std::string msg = hint.empty() ? "unexpected token" : "expected " + hint;
        throw Exception::Parse(msg, {current_.line, current_.column});
    }
    return consume();
}

Token Tokenizer::expect_keyword(const std::string& kw) {
    if (current_.type != TokenType::Keyword || current_.text != kw) {
        throw Exception::Parse("expected '" + kw + "'", {current_.line, current_.column});
    }
    return consume();
}

bool Tokenizer::peek_keyword(const std::string& kw) const {
    return current_.type == TokenType::Keyword && current_.text == kw;
}

bool Tokenizer::at_eof() const {
    return current_.type == TokenType::Eof;
}

std::pair<size_t, size_t> Tokenizer::location() const {
    return {current_.line, current_.column};
}

// ── Internal scanning ─────────────────────────────────────────────────────────

char Tokenizer::peek_char() const {
    if (pos_ >= source_.size()) return '\0';
    return source_[pos_];
}

char Tokenizer::consume_char() {
    char c = source_[pos_++];
    if (c == '\n') { ++line_; col_ = 0; }
    else           { ++col_; }
    return c;
}

bool Tokenizer::starts_with(const std::string& prefix) const {
    return source_.compare(pos_, prefix.size(), prefix) == 0;
}

void Tokenizer::skip_whitespace_and_comments() {
    while (pos_ < source_.size()) {
        char c = peek_char();
        // Whitespace
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
            consume_char();
            continue;
        }
        // Line comment ;;
        if (c == ';' && pos_ + 1 < source_.size() && source_[pos_ + 1] == ';') {
            pos_ += 2; col_ += 2;
            while (pos_ < source_.size() && source_[pos_] != '\n')
                consume_char();
            continue;
        }
        // Block comment (;...;) — nestable
        if (c == '(' && pos_ + 1 < source_.size() && source_[pos_ + 1] == ';') {
            size_t depth = 1;
            size_t start_line = line_, start_col = col_;
            pos_ += 2; col_ += 2;
            while (pos_ < source_.size() && depth > 0) {
                if (source_[pos_] == '(' && pos_ + 1 < source_.size() && source_[pos_ + 1] == ';') {
                    ++depth; pos_ += 2; col_ += 2;
                } else if (source_[pos_] == ';' && pos_ + 1 < source_.size() && source_[pos_ + 1] == ')') {
                    --depth; pos_ += 2; col_ += 2;
                } else {
                    consume_char();
                }
            }
            if (depth > 0) {
                throw Exception::Parse("unterminated block comment", {start_line, start_col});
            }
            continue;
        }
        break;
    }
}

// Scan the next Hex/Decimal number suffix (after sign and optional 0x already consumed).
// Returns the digits string. Allows underscores between digits.
static std::string scan_num_suffix(const std::string& src, size_t& pos, bool hex) {
    std::string s;
    while (pos < src.size()) {
        char c = src[pos];
        bool valid = hex ? is_hexdigit(c) : is_digit(c);
        if (valid) { s += c; ++pos; }
        else if (c == '_' && !s.empty() && pos + 1 < src.size() &&
                 (hex ? is_hexdigit(src[pos+1]) : is_digit(src[pos+1])))
        {
            s += c; ++pos;
        }
        else break;
    }
    return s;
}

Token Tokenizer::scan_next() {
    // Save position state before skipping (for accurate column after skip)
    skip_whitespace_and_comments();

    if (pos_ >= source_.size()) {
        return Token{TokenType::Eof, "", line_, col_};
    }

    size_t start_line = line_;
    size_t start_col  = col_;
    char c = peek_char();

    // Parentheses
    if (c == '(') {
        consume_char();
        return Token{TokenType::LParen, "(", start_line, start_col};
    }
    if (c == ')') {
        consume_char();
        return Token{TokenType::RParen, ")", start_line, start_col};
    }

    // String literal
    if (c == '"') {
        return scan_string();
    }

    // Id: starts with $
    if (c == '$') {
        return scan_id();
    }

    // Signed number or sign-starting keyword
    if ((c == '+' || c == '-') && pos_ + 1 < source_.size() && sign_starts_number(source_, pos_ + 1)) {
        return scan_number(start_line, start_col);
    }

    // Unsigned number
    if (is_digit(c)) {
        return scan_number(start_line, start_col);
    }

    // Keyword / offset= / align=
    return scan_keyword(start_line, start_col);
}

Token Tokenizer::scan_string() {
    size_t start_line = line_, start_col = col_;
    std::string text;
    text += consume_char(); // opening "
    while (pos_ < source_.size()) {
        char c = peek_char();
        if (c == '"') { text += consume_char(); break; }
        if (c == '\\') {
            text += consume_char(); // backslash
            if (pos_ < source_.size()) text += consume_char(); // escaped char
            // handle \u{ ... } — scan until }
            if (text.size() >= 2 && text[text.size()-1] == '{') {
                while (pos_ < source_.size() && peek_char() != '}')
                    text += consume_char();
                if (pos_ < source_.size()) text += consume_char(); // '}'
            }
        } else {
            text += consume_char();
        }
    }
    return Token{TokenType::String, text, start_line, start_col};
}

Token Tokenizer::scan_id() {
    size_t start_line = line_, start_col = col_;
    std::string text;
    text += consume_char(); // $
    while (pos_ < source_.size() && is_keyword_char(peek_char()))
        text += consume_char();
    return Token{TokenType::Id, text, start_line, start_col};
}

Token Tokenizer::scan_number(size_t start_line, size_t start_col) {
    std::string text;
    bool is_float = false;

    // Optional sign
    char c = peek_char();
    if (c == '+' || c == '-') {
        text += consume_char();
        c = peek_char();
    }

    // inf / nan / nan:0x...
    if (source_.compare(pos_, 3, "inf") == 0) {
        text += consume_char(); text += consume_char(); text += consume_char();
        return Token{TokenType::Float, text, start_line, start_col};
    }
    if (source_.compare(pos_, 3, "nan") == 0) {
        text += consume_char(); text += consume_char(); text += consume_char();
        // optional :0x<hexnum>
        if (pos_ < source_.size() && source_[pos_] == ':') {
            text += consume_char(); // :
            if (pos_ + 1 < source_.size() && source_[pos_] == '0' && source_[pos_+1] == 'x') {
                text += consume_char(); text += consume_char(); // 0x
                while (pos_ < source_.size() && (is_hexdigit(peek_char()) ||
                       (peek_char() == '_' && pos_+1 < source_.size() && is_hexdigit(source_[pos_+1]))))
                    text += consume_char();
            }
        }
        return Token{TokenType::Float, text, start_line, start_col};
    }

    // Hex: 0x...
    if (source_.compare(pos_, 2, "0x") == 0) {
        text += consume_char(); text += consume_char(); // 0x
        // hex significand
        std::string hex_int = scan_num_suffix(source_, pos_, true);
        text += hex_int;
        // optional fractional part
        if (pos_ < source_.size() && source_[pos_] == '.') {
            is_float = true;
            text += consume_char();
            text += scan_num_suffix(source_, pos_, true);
        }
        // optional exponent P/p
        if (pos_ < source_.size() && (source_[pos_] == 'P' || source_[pos_] == 'p')) {
            is_float = true;
            text += consume_char();
            if (pos_ < source_.size() && (source_[pos_] == '+' || source_[pos_] == '-'))
                text += consume_char();
            text += scan_num_suffix(source_, pos_, true);
        }
        return Token{is_float ? TokenType::Float : TokenType::Integer, text, start_line, start_col};
    }

    // Decimal
    text += scan_num_suffix(source_, pos_, false);
    // optional fractional or exponent
    if (pos_ < source_.size() && source_[pos_] == '.') {
        is_float = true;
        text += consume_char();
        text += scan_num_suffix(source_, pos_, false);
    }
    if (pos_ < source_.size() && (source_[pos_] == 'E' || source_[pos_] == 'e')) {
        is_float = true;
        text += consume_char();
        if (pos_ < source_.size() && (source_[pos_] == '+' || source_[pos_] == '-'))
            text += consume_char();
        text += scan_num_suffix(source_, pos_, false);
    }
    return Token{is_float ? TokenType::Float : TokenType::Integer, text, start_line, start_col};
}

Token Tokenizer::scan_keyword(size_t start_line, size_t start_col) {
    std::string text;
    while (pos_ < source_.size() && is_keyword_char(peek_char()))
        text += consume_char();

    if (text.empty()) {
        // Unknown character — advance past it and throw
        char bad = consume_char();
        throw Exception::Parse(std::string("unknown token: ") + bad, {start_line, start_col});
    }

    // offset=<integer>
    if (text == "offset" && pos_ < source_.size() && source_[pos_] == '=') {
        consume_char(); // '='
        size_t val_start = pos_;
        // scan the integer value (with optional sign and hex)
        char c = peek_char();
        std::string val;
        if (c == '+' || c == '-') val += consume_char();
        if (pos_ + 1 < source_.size() && source_[pos_] == '0' && source_[pos_+1] == 'x') {
            val += consume_char(); val += consume_char();
            val += scan_num_suffix(source_, pos_, true);
        } else {
            val += scan_num_suffix(source_, pos_, false);
        }
        return Token{TokenType::Offset, val, start_line, start_col};
    }

    // align=<integer>
    if (text == "align" && pos_ < source_.size() && source_[pos_] == '=') {
        consume_char(); // '='
        std::string val;
        if (peek_char() == '+' || peek_char() == '-') val += consume_char();
        if (pos_ + 1 < source_.size() && source_[pos_] == '0' && source_[pos_+1] == 'x') {
            val += consume_char(); val += consume_char();
            val += scan_num_suffix(source_, pos_, true);
        } else {
            val += scan_num_suffix(source_, pos_, false);
        }
        return Token{TokenType::Align, val, start_line, start_col};
    }

    return Token{TokenType::Keyword, text, start_line, start_col};
}

void Tokenizer::advance_current() {
    if (next_valid_) {
        current_ = next_;
        next_valid_ = false;
    } else {
        current_ = scan_next();
    }
}
