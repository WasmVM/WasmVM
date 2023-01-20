#ifndef WASMVM_PP_PARSE_TOKEN_DEF
#define WASMVM_PP_PARSE_TOKEN_DEF

#include <string>
#include <utility>
#include <optional>

#include <Util.hpp>
#include <Types.hpp>

namespace WasmVM {

namespace Exception {
    struct unexpected_keyword;
}

namespace Token {
    struct Id;
    struct String;
    struct Number;
    struct ParenL;
    struct ParenR;
    struct KeywordBase;
}

using TokenType = std::variant<
    Token::ParenL,
    Token::ParenR,
    Token::Id,
    Token::String,
    Token::Number,
    Token::KeywordBase
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

struct KeywordBase : public TokenBase {
    KeywordBase(Location loc, std::string value);
};

template <conststr K, bool Required = false>
struct Keyword : public KeywordBase {
    Keyword(Location loc) : KeywordBase(loc, K.value){}

    static std::optional<Keyword> get(TokenIter& begin, const TokenIter& end){
        if(begin != end && std::holds_alternative<KeywordBase>(*begin)){
            KeywordBase& keyword = std::get<KeywordBase>(*(begin++));
            if(keyword.value == K.value){
                return Keyword(keyword.location);
            }else if (Required){
                throw Exception::unexpected_keyword(keyword.location, keyword.value, K.value);
            }
        }
        return std::nullopt;
    }
};

}

}

#endif