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
    struct unknown_identifier : public Parse {
        unknown_identifier(Token::Location location, std::string message);
    };
    struct invalid_functype : public Parse {
        invalid_functype(Token::Location location, std::string message);
    };
    struct invalid_limit : public Parse {
        invalid_limit(Token::Location location, std::string message);
    };
    struct duplicated_identifier : public Parse {
        duplicated_identifier(Token::Location location, std::string message);
    };
    struct index_out_of_range : public Parse {
        index_out_of_range(Token::Location location, std::string message);
    };
    struct block_id_mismatch : public Parse {
        block_id_mismatch(Token::Location location, std::string message);
    };
    struct invalid_folded_instruction : public Parse {
        invalid_folded_instruction(Token::Location location);
    };
    struct invalid_immediate_value : public Parse {
        invalid_immediate_value(Token::Location location, std::string message);
    };
    struct brackets_not_close : public Parse {
        brackets_not_close(Token::Location location, std::string message);
    };
}

namespace Parse {

template <typename T>
concept parseable = requires(TokenIter& begin, TokenHolder& holder) {
    {T::get(begin, holder)} -> std::convertible_to<std::optional<T>>;
};

template<parseable... T>
class Rule : public std::tuple<T...> {
    Rule(T... values) : std::tuple<T...>(values...){}
public:

    static std::optional<Rule<T...>> get(TokenIter& begin, TokenHolder& holder){
        TokenIter it = begin;
        if(((T::get(it, holder).has_value()) && ...)){
            return Rule<T...>((*T::get(begin, holder))...);
        }else{
            return std::optional<Rule<T...>>();
        }
    }
};

template<parseable T>
struct Optional : public std::optional<T> {

    Optional(std::optional<T>& val) : std::optional<T>(val){}

    static std::optional<Optional<T>> get(TokenIter& begin, TokenHolder& holder){
        TokenIter it = begin;
        auto result = T::get(it, holder);
        if(result){
            begin = it;
        }
        return Optional<T>(result);
    }
};

template<parseable T, size_t Min = 0, size_t Max = SIZE_MAX>
    requires (Min <= Max)
struct Repeat : public std::vector<T> {
    static std::optional<Repeat<T, Min, Max>> get(TokenIter& begin, TokenHolder& holder){
        Repeat<T, Min, Max> result;
        for(size_t i = 0; i < Max; ++i){
            TokenIter iter_in = begin;
            std::optional<T> item = T::get(iter_in, holder);
            if(item){
                begin = iter_in;
                result.emplace_back(*item);
            }else{
                break;
            }
        }
        if(result.size() < Min){
            return std::nullopt;
        }
        return result;
    }
};

template<parseable... T>
struct OneOf : public std::variant<T...>{

    template <class U>
    OneOf(U arg) : std::variant<T...>(arg){}

    static std::optional<OneOf<T...>> get(TokenIter& begin, TokenHolder& holder){
        std::optional<OneOf<T...>> result;
        if((attempt<T>(result, begin, holder) || ...)){
           return result;
        }else{
            return std::nullopt;
        }
    }
private:
    template<parseable U>
    static bool attempt(std::optional<OneOf<T...>>& res, TokenIter& begin, TokenHolder& holder){
        TokenIter it = begin;
        std::optional<U> result = U::get(it, holder);
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