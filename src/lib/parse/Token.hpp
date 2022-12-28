#ifndef WASMVM_PP_PARSE_TOKEN_DEF
#define WASMVM_PP_PARSE_TOKEN_DEF

#include <string>
#include <variant>
#include <map>
#include <concepts>
#include <Types.hpp>

namespace WasmVM {

namespace TokenType {

struct LeftParen;
struct RightParen;
struct Id;
struct Number;
struct String;
struct Keyword;

using Token = std::variant<
    std::monostate,
    LeftParen, RightParen, Id, Number, String, Keyword
>;

struct LeftParen {
    static bool match(std::string str);
    static Token create(std::string);
};

struct RightParen {
    static bool match(std::string str);
    static Token create(std::string);
};

struct Id {
    static bool match(std::string str);
    static Token create(std::string str);
    std::string value;
};

struct Number {
    static bool match(std::string str);
    static Token create(std::string str);
    Value value;
};

struct String {
    static bool match(std::string str);
    static Token create(std::string str);
    std::string value;
};

struct Keyword {

    enum Type {
        module,
    };

    static bool match(std::string str);
    static Token create(std::string str);
    Keyword(std::string str);
    Keyword(Type type);
    Type type;

private:
    static std::map<std::string, Type> typemap;
};

template <typename T>
concept token_type = requires {
    { T::match(std::string()) } -> std::convertible_to<bool>;
    { T::create(std::string()) } -> std::convertible_to<Token>;
};

static_assert(token_type<LeftParen>);
static_assert(token_type<RightParen>);
static_assert(token_type<Id>);
static_assert(token_type<Number>);
static_assert(token_type<String>);
static_assert(token_type<Keyword>);

}

using Token = TokenType::Token;

}

#endif