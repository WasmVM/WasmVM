#ifndef WASMVM_JSON_DEF
#define WASMVM_JSON_DEF

#include <variant>
#include <string>
#include <iostream>
#include <type_traits>

namespace Json {

struct newline {
    newline(bool toggle = true) : toggle(toggle){};
    bool toggle;
    static long index;
};
std::ostream& operator<<(std::ostream&, const newline&);

struct indent {
    constexpr static char default_indent[] = "\t";
    indent(std::string value = default_indent) : value(value){};
    std::string value;
    static long index;
};
std::ostream& operator<<(std::ostream&, const indent&);

class Value {

    using Null = std::monostate;
    using Bool = bool;
    std::variant<Null, Bool> value;

    friend std::istream& operator>>(std::istream&, Value&);
    friend std::ostream& operator<<(std::ostream&, const Value&);

public:
};

std::istream& operator>>(std::istream&, Value&);
std::ostream& operator<<(std::ostream&, const Value&);

} // namespace Json

#endif