#ifndef WASMVM_PP_PARSE_Slicer_DEF
#define WASMVM_PP_PARSE_Slicer_DEF

#include <istream>
#include <stack>
#include <string>

#include "Token.hpp"

namespace WasmVM {

struct Stream {

    Stream(std::istream& stream);

    char get();
    char peek();

private:
    std::istream& stream;
    std::stack<char> buf;
};

struct Slicer : public Stream{

    Slicer(std::istream& stream);

    const std::string& operator*() const;
    Slicer& operator++();
    Slicer operator++(int);
    operator bool();

private:
    std::string current;
};

}

#endif