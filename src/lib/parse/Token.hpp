#ifndef WASMVM_PP_PARSE_TOKEN_DEF
#define WASMVM_PP_PARSE_TOKEN_DEF

#include <string>
#include <functional>
#include <utility>

#include <Types.hpp>
#include <Util.hpp>

namespace WasmVM {

namespace Token {

using Location = std::pair<size_t, size_t>;

struct TokenBase {
    TokenBase(Location& loc) : location(loc){}
    Location location;
};

template<char C>
    requires ((C == '(') || (C == ')'))
struct Paren : public TokenBase {
    Paren(Location loc) : TokenBase(loc){}
    constexpr static char value = C;
};

struct Id : public TokenBase {
    Id(Location loc) : TokenBase(loc){}
    std::string value;
};

struct Number : public TokenBase {
    Number(Location loc) : TokenBase(loc){}
    Value value;
};

struct String : public TokenBase {
    String(Location loc) : TokenBase(loc){}
    std::string value;
};

struct Keyword : public TokenBase {
    Keyword(Location loc) : TokenBase(loc){}
    size_t value;
};

}
}

#endif