#include <iostream>
#include <fstream>
#include <exception.hpp>
#include "Lexer.hpp"

int main(int argc, char const *argv[]){
    std::ifstream fin(argv[1]);
    WasmVM::Lexer lexer(argv[1], fin);
    try{
        WasmVM::Token token = lexer.get();
        while(!std::holds_alternative<std::monostate>(token)){
            token = lexer.get();
        }
    }catch(WasmVM::Exception::Parse& e){
        std::cerr << argv[1] << ":" << e.location.first << ":" << e.location.second << " error: " << e.what() << std::endl;
    }catch(WasmVM::UnknownToken& e){
        std::cerr << e.what() << std::endl;
    }
    fin.close();
    return 0;
}
