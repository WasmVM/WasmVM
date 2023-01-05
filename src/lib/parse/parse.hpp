#ifndef WASMVM_PP_PARSE_LEXER_DEF
#define WASMVM_PP_PARSE_LEXER_DEF

#include <string>
#include <list>
#include <variant>
#include <exception.hpp>

#include "Token.hpp"

namespace WasmVM {

using TokenVar = std::variant<
    Token::Paren<'('>,
    Token::Paren<')'>,
    Token::Id,
    Token::String,
    Token::Number,
    Token::Keyword
>;

std::list<TokenVar> tokenize(std::string_view src);

namespace Exception {
struct string_not_close : public Parse {
    string_not_close(Token::Location location) : Parse("string not close", location) {}
};
struct block_comment_not_close : public Parse {
    block_comment_not_close(Token::Location location) : Parse("block comment not close", location) {}
};
}

}

#endif