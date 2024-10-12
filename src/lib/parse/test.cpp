#include <iostream>
#include <fstream>
#include <exception.hpp>
#include <WasmVM.hpp>
#include <antlr4-runtime.h>

// #include "../dump/dump.hpp"
#include <WatLexer.h>
#include <WatParser.h>

int main(int argc, char const *argv[]){
    std::ifstream fin(argv[1]);
    antlr4::ANTLRInputStream input(fin);
    WasmVM::WatLexer lexer(&input);
    antlr4::CommonTokenStream tokens(&lexer);
    WasmVM::WatParser parser(&tokens);
    
    parser.module();
    // try{
    //     WasmVM::Lexer lexer(argv[1], fin);
    //     WasmVM::Parser parser(lexer);
    //     WasmVM::WasmModule module = parser.parse();
    //     WasmVM::module_dump(module, std::cout);
    // }catch(WasmVM::Exception::Parse& e){
    //     std::cerr << argv[1] << ":" << e.location.first << ":" << e.location.second << " error: " << e.what() << std::endl;
    // }catch(WasmVM::ParseError& e){
    //     std::cerr << argv[1] << ":" << e.pos.line << ":" << e.pos.column << " error: " << e.what() << std::endl;
    // }catch(WasmVM::UnknownToken& e){
    //     std::cerr << e.what() << std::endl;
    // }
    fin.close();
    return 0;
}
