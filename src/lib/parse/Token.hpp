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
    TokenBase(Location& loc);
    Location location;
};

template<char C>
    requires ((C == '(') || (C == ')'))
struct Paren : public TokenBase {
    Paren(Location loc);
    constexpr static char value = C;
};

struct Id : public TokenBase {
    Id(Location loc, std::string str);
    std::string value;
};

struct Number : public TokenBase {
    static std::optional<Number> create(Location loc, std::string str);
    Value value;
private:
    Number(Location loc, Value value);
};

struct String : public TokenBase {
    String(Location loc, std::string str);
    std::string value;
};

struct Keyword : public TokenBase {
    static std::optional<Keyword> create(Location loc, std::string str);
    size_t value;
private:
    Keyword(Location loc, std::string value);
};

}
}

#endif