#include <fstream>
#include <iostream>

#include "Lexer.hpp"

using namespace WasmVM;

int main(int argc, char const *argv[])
{
    std::ifstream fin(argv[1]);

    for(Slicer slicer(fin); slicer; ++slicer){
        std::cout << *slicer << std::endl;
    }

    fin.close();
    return 0;
}
