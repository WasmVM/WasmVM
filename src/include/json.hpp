#ifndef WASMVM_JSON_DEF
#define WASMVM_JSON_DEF

#include <variant>
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <type_traits>
#include <cstdint>
#include <exception.hpp>

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

struct Value {

    using Null = std::monostate;
    using Bool = bool;
    using Number = double;
    using String = std::string;
    using Array = std::vector<Value>;
    using Object = std::map<String, Value>;
    // A number written without a fraction or an exponent that fits in 64 bits
    // parses as Integer; every other number is a Number (double).
    using Integer = int64_t;

    Value() : value(Object()){}

    enum class Type {
        Null, Bool, Number, String, Array, Object, Integer
    };
    constexpr Type type(){
        return static_cast<const Type>(value.index());
    }
    template<typename T> T& get(){
        if(std::holds_alternative<T>(value)){
            return std::get<T>(value);
        }else{
            throw WasmVM::Exception::Exception("JSON: value type mismatch");
        }
    }
    template<class Visitor> void visit(Visitor&& visitor){
        std::visit(visitor, value);
    }
    template<class R, class Visitor> R visit(Visitor&& visitor){
        return std::visit(visitor, value);
    }

private:
    std::variant<Null, Bool, Number, String, Array, Object, Integer> value;

    friend std::istream& operator>>(std::istream&, Value&);
    friend std::ostream& operator<<(std::ostream&, const Value&);
};

std::istream& operator>>(std::istream&, Value&);
std::ostream& operator<<(std::ostream&, const Value&);

} // namespace Json

#endif