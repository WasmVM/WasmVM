#include <iostream>
#include <fstream>
#include <exception.hpp>
#include <WasmVM.hpp>
// #include "Lexer.hpp"
// #include "Parser.hpp"
#include "../dump/dump.hpp"

int main(int argc, char const *argv[]){
    std::ifstream fin(argv[1]);
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
