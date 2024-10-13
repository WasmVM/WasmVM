#include <iostream>
#include <fstream>
#include <exception.hpp>
#include <WasmVM.hpp>
#include <antlr4-runtime.h>

// #include "../dump/dump.hpp"
#include <WatLexer.h>
#include <WatParser.h>
#include "ErrorListener.hpp"
#include "Visitor.hpp"

int main(int argc, char const *argv[]){
    std::ifstream fin(argv[1]);
    antlr4::ANTLRInputStream input(fin);
    WasmVM::WatLexer lexer(&input);
    WasmVM::LexerErrorListener lexerErrorListener;
    WasmVM::ParserErrorListener parserErrorListener;
    antlr4::CommonTokenStream tokens(&lexer);
    WasmVM::WatParser parser(&tokens);

    lexer.removeErrorListeners();
    lexer.addErrorListener(&lexerErrorListener);
    parser.removeErrorListeners();
    parser.addErrorListener(&parserErrorListener);
    try{
        WasmVM::Visitor visitor;
        WasmVM::WasmModule module = std::any_cast<WasmVM::WasmModule>(visitor.visitModule(parser.module()));
        WasmVM::module_dump(module, std::cout);
    }catch(WasmVM::Exception::Parse& e){
        std::cerr << argv[1] << ":" << e.location.first << ":" << e.location.second << " error: " << e.what() << std::endl;
    }
    return 0;
}
