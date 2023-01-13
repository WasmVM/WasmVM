#ifndef WASMVM_PP_PARSE_LEXER_DEF
#define WASMVM_PP_PARSE_LEXER_DEF

#include <string>
#include <list>
#include <utility>
#include <exception.hpp>

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

template<typename... T>
class Rule : public std::tuple<std::optional<T>...> {
    Rule(std::optional<T>... values) : std::tuple<std::optional<T>...>(values...){}
public:
    static Rule<T...> get(TokenIter begin, TokenIter end){
        return Rule<T...>(T::get(begin, end)...);
    }
};

}

#endif