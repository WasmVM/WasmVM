#ifndef WASMVM_PP_PARSE_TOKEN_DEF
#define WASMVM_PP_PARSE_TOKEN_DEF

#include <string>
#include <utility>
#include <optional>

#include <Util.hpp>
#include <Types.hpp>

namespace WasmVM {

namespace Token {
    struct Id;
    struct String;
    struct Number;
    struct Keyword;
    struct ParenL;
    struct ParenR;
}

using TokenType = std::variant<
    Token::ParenL,
    Token::ParenR,
    Token::Id,
    Token::String,
    Token::Number,
    Token::Keyword
>;

using TokenIter = std::list<TokenType>::iterator;

namespace Token {

using Location = std::pair<size_t, size_t>;

struct TokenBase {
    TokenBase(Location& loc, std::string value);
    Location location;
    std::string value;
};

struct ParenL : public TokenBase {
    ParenL(Location loc);
    static std::optional<ParenL> get(TokenIter& begin, const TokenIter& end);
};

struct ParenR : public TokenBase {
    ParenR(Location loc);
    static std::optional<ParenR> get(TokenIter& begin, const TokenIter& end);
};

struct Id : public TokenBase {
    Id(Location loc, std::string value);
    static std::optional<Id> get(TokenIter& begin, const TokenIter& end);
};

struct Number : public TokenBase {
    static std::optional<Number> create(Location loc, std::string str);
    std::optional<Number> get(TokenIter& begin, const TokenIter& end);
private:
    Number(Location loc, std::string value);
};

struct String : public TokenBase {
    String(Location loc, std::string value);
    static std::optional<String> get(TokenIter& begin, const TokenIter& end);
};

struct Keyword : public TokenBase {
    Keyword(Location loc, std::string value);
    static std::optional<Keyword> get(TokenIter& begin, const TokenIter& end);
};

}

}

#endif