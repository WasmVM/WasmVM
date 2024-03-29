#ifndef WASMVM_PP_PARSE_TOKEN_DEF
#define WASMVM_PP_PARSE_TOKEN_DEF

#include <string>
#include <list>
#include <utility>
#include <optional>
#include <iostream>

#include <Util.hpp>
#include <Types.hpp>

namespace WasmVM {

namespace Token {
    struct Id;
    struct String;
    struct Number;
    struct ParenL;
    struct ParenR;
    struct MemArgBase;
    struct KeywordBase;
    using Location = std::pair<size_t, size_t>;
}

namespace Exception {
    struct unexpected_keyword : public Parse {
        unexpected_keyword(Token::Location location, std::string token, std::string expected);
    };
    struct unexpected_token : public Parse {
        unexpected_token(Token::Location location, std::string message);
    };
    struct invalid_character : public Parse {
        invalid_character(Token::Location location, std::string message);
    };
}

using TokenType = std::variant<
    Token::ParenL,
    Token::ParenR,
    Token::Id,
    Token::String,
    Token::Number,
    Token::MemArgBase,
    Token::KeywordBase
>;

std::list<TokenType> tokenize(std::istream& src);

using TokenIter = std::list<TokenType>::iterator;

struct TokenHolder {
    TokenHolder(const TokenIter& begin, const TokenIter& end): last(begin), end(end) {}
    bool has_next(TokenIter& it);
    TokenIter next(TokenIter& it);
    Exception::Parse error();
protected:
    TokenIter last;
    const TokenIter end;
};

namespace Token {

struct TokenBase {
    TokenBase(Location& loc, std::string value);
    Location location;
    std::string value;
};

struct ParenL : public TokenBase {
    ParenL(Location loc);
    static std::optional<ParenL> get(TokenIter& begin, TokenHolder& holder);
};

struct ParenR : public TokenBase {
    ParenR(Location loc);
    static std::optional<ParenR> get(TokenIter& begin, TokenHolder& holder);
};

struct Id : public TokenBase {
    Id(Location loc, std::string value);
    static std::optional<Id> get(TokenIter& begin, TokenHolder& holder);
};

struct Number : public TokenBase {
    Number(Location loc, std::string value);
    static std::optional<Number> get(TokenIter& begin, TokenHolder& holder);
    template<typename T> T unpack();
};

struct MemArgBase : public TokenBase {
    MemArgBase(Location loc, std::string key, std::string value);
    template<typename T> T unpack();
    std::string key;
};

struct String : public TokenBase {
    String(Location loc, std::string value);
    static std::optional<String> get(TokenIter& begin, TokenHolder& holder);
};

struct KeywordBase : public TokenBase {
    KeywordBase(Location loc, std::string value);
};

template <conststr K>
struct MemArg : public MemArgBase {
    MemArg(Location loc, std::string key, std::string value) : MemArgBase(loc, key, value){}

    static std::optional<MemArg> get(TokenIter& begin, TokenHolder& holder){
        if(holder.has_next(begin) && std::holds_alternative<MemArgBase>(*begin)){
            MemArgBase& memarg = std::get<MemArgBase>(*holder.next(begin));
            if(memarg.key == K.value){
                return MemArg(memarg.location, memarg.key, memarg.value);
            }
        }
        return std::nullopt;
    }
};

template <conststr K, bool Required = false>
struct Keyword : public KeywordBase {
    Keyword(Location loc) : KeywordBase(loc, K.value){}

    static std::optional<Keyword> get(TokenIter& begin, TokenHolder& holder){
        if(holder.has_next(begin) && std::holds_alternative<KeywordBase>(*begin)){
            KeywordBase& keyword = std::get<KeywordBase>(*holder.next(begin));
            if(keyword.value == K.value){
                return Keyword(keyword.location);
            }else if (Required){
                throw Exception::unexpected_keyword(keyword.location, keyword.value, K.value);
            }
        }
        return std::nullopt;
    }
};

} // namespace Token

} // namespace WasmVM

#endif