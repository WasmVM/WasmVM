#ifndef WASMVM_PP_PARSE_LEXER_DEF
#define WASMVM_PP_PARSE_LEXER_DEF

#include <string>
#include <list>
#include <utility>
#include <vector>
#include <concepts>

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
    struct unexpected_keyword : public Parse {
        unexpected_keyword(Token::Location location, std::string token, std::string expected);
    };
    struct invalid_functype : public Parse {
        invalid_functype(Token::Location location, std::string message);
    };
}

namespace Parse {

template <typename T>
concept parseable = requires(TokenIter& begin, const TokenIter& end) {
    {T::get(begin, end)} -> std::convertible_to<std::optional<T>>;
};

template<parseable... T>
class Rule : public std::tuple<T...> {
    Rule(T... values) : std::tuple<T...>(values...){}
public:

    static std::optional<Rule<T...>> get(TokenIter& begin, const TokenIter& end){
        TokenIter it = begin;
        if(((T::get(it, end).has_value()) && ...)){
            return Rule<T...>((*T::get(begin, end))...);
        }else{
            return std::optional<Rule<T...>>();
        }
    }
};

template<parseable T>
struct Optional : public std::optional<T> {
    static std::optional<Optional<T>> get(TokenIter& begin, const TokenIter& end){
        TokenIter it = begin;
        auto result = T::get(it, end);
        if(result){
            begin = it;
        }
        return Optional<T>(result);
    }
};

template<parseable T, size_t Min = 0, size_t Max = SIZE_MAX>
    requires (Min <= Max)
struct Repeat : public std::vector<T> {
    static std::optional<Repeat<T, Min, Max>> get(TokenIter& begin, const TokenIter& end){
        Repeat<T, Min, Max> result;
        TokenIter it = begin;
        for(size_t i = 0; i < Max; ++i){
            TokenIter iter_in = it;
            std::optional<T> item = T::get(iter_in, end);
            if(item){
                it = iter_in;
                result.emplace_back(*item);
            }else{
                break;
            }
        }
        if(result.size() < Min){
            return std::nullopt;
        }
        begin = it;
        return result;
    }
};

template<parseable T, size_t N>
using Repeat_N = Repeat<T, N, N>;

template<parseable... T>
struct OneOf : public std::variant<T...>{

    template <class U>
    OneOf(U arg) : std::variant<T...>(arg){}

    static std::optional<OneOf<T...>> get(TokenIter& begin, const TokenIter& end){
        std::optional<OneOf<T...>> result;
        if((attempt<T>(result, begin, end) || ...)){
           return result; 
        }else{
            return std::nullopt;
        }
    }
private:
    template<parseable U>
    static bool attempt(std::optional<OneOf<T...>>& res, TokenIter& begin, const TokenIter& end){
        TokenIter it = begin;
        std::optional<U> result = U::get(it, end);
        if(result){
            begin = it;
            res.emplace(OneOf<T...>(*result));
            return true;
        }else{
            return false;
        }
    }
};

}

}

#endif