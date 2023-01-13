#ifndef WASMVM_PP_PARSE_LEXER_DEF
#define WASMVM_PP_PARSE_LEXER_DEF

#include <string>
#include <list>
#include <utility>

#include <exception.hpp>
#include <structures/WasmModule.hpp>
#include "Token.hpp"

namespace WasmVM {

std::list<TokenType> tokenize(std::string_view src);

namespace Exception {
    struct string_not_close : public Parse {
        string_not_close(Token::Location location);
    };
    struct block_comment_not_close : public Parse {
        block_comment_not_close(Token::Location location);
    };
    struct unknown_token : public Parse {
        unknown_token(Token::Location location, std::string token);
    };
}

namespace Parse {

template<typename... T>
class Rule : public std::tuple<std::optional<T>...> {
    Rule(std::optional<T>... values) : std::tuple<std::optional<T>...>(values...){}
public:
    static Rule<T...> get(TokenIter& begin, const TokenIter& end){
        return Rule<T...>(T::get(begin, end)...);
    }
};

template<typename T>
class Optional : public std::optional<T> {
    Optional(T value) : std::optional<T>(value){}
    Optional(std::optional<T> value) : std::optional<T>(value){}
    Optional(std::nullopt_t = std::nullopt) : std::optional<T>(){}
public:
    static Optional<T> get(TokenIter& begin, const TokenIter& end){
        TokenIter it = begin;
        Optional<T> result(T::get(begin, end));
        if(result){
            begin = it;
        }
        return result;
    }
};

}

}

#endif