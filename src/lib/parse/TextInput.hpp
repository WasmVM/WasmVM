#ifndef WASMVM_PP_PARSE_TEXTINPUT_DEF
#define WASMVM_PP_PARSE_TEXTINPUT_DEF

#include <istream>
#include <stack>
#include <string>

struct TextInput {

    TextInput(std::istream& stream);

    char get();
    char peek();
    bool eof();
    TextInput& putback(char ch);

private:
    std::istream& stream;
    std::stack<char> buf;
};

#endif