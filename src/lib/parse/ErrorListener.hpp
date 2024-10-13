#ifndef WASMVM_Parse_ErrorListener
#define WASMVM_Parse_ErrorListener

#include <antlr4-runtime.h>

namespace WasmVM {

struct LexerErrorListener : antlr4::BaseErrorListener {
    void syntaxError(antlr4::Recognizer *recognizer, antlr4::Token * offendingSymbol, size_t line, size_t charPositionInLine, const std::string &msg, std::exception_ptr e) override;
};

struct ParserErrorListener : antlr4::BaseErrorListener {
    void syntaxError(antlr4::Recognizer *recognizer, antlr4::Token * offendingSymbol, size_t line, size_t charPositionInLine, const std::string &msg, std::exception_ptr e) override;
};

}

#endif