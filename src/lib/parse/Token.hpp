#ifndef WASMVM_PP_PARSE_TOKEN_DEF
#define WASMVM_PP_PARSE_TOKEN_DEF

#include <string>
#include <utility>
#include <optional>

#include <Util.hpp>
#include <Types.hpp>

namespace WasmVM {

namespace Token {

using Location = std::pair<size_t, size_t>;

struct TokenBase {
    TokenBase(Location& loc, std::string value);
    Location location;
    std::string value;
};

template<char C>
    requires ((C == '(') || (C == ')'))
struct Paren : public TokenBase {
    Paren(Location loc);
};

struct Id : public TokenBase {
    Id(Location loc, std::string value);
};

struct Number : public TokenBase {
    static std::optional<Number> create(Location loc, std::string str);
private:
    Number(Location loc, std::string value);
};

struct String : public TokenBase {
    String(Location loc, std::string value);
};

struct Keyword : public TokenBase {
    Keyword(Location loc, std::string value);
};

}

using TokenType = std::variant<
    Token::Paren<'('>,
    Token::Paren<')'>,
    Token::Id,
    Token::String,
    Token::Number,
    Token::Keyword
>;

}

#endif