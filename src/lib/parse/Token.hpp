#ifndef WASMVM_PP_PARSE_TOKEN_DEF
#define WASMVM_PP_PARSE_TOKEN_DEF

#include <string>

#include <Types.hpp>
#include <Util.hpp>
#include <functional>

namespace WasmVM {

namespace TokenType {

template<char C>
    requires ((C == '(') || (C == ')'))
struct Paren {
    constexpr static char value = C;
};

struct Id {
    std::string value;
};

struct Number {
    Value value;
};

struct String {
    std::string value;
};

struct Keyword {
    size_t value;
};

}
}

#endif