// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <fstream>
#include <filesystem>
#include <iostream>
#include <string>
#include <ranges>
#include <map>
#include <utility>

#include <WasmVM.hpp>
#include <exception.hpp>

using namespace WasmVM;

int main(int argc, char const *argv[])
{
    Exception::Warning::regist([](std::string message){
        std::cerr << "warning: " << message << std::endl;
    });

    std::filesystem::path path(argv[1]);
    std::ifstream fin(path);
    size_t fsize = std::filesystem::file_size(path);
    std::string src(fsize, ' ');
    fin.read(src.data(), fsize);
    fin.close();

    try {
        WasmModule result = module_parse(src);

        std::cout << module_dump(result);
        
    }catch(Exception::Parse &e){
        std::cerr << path.string() << ":" << e.location.first << ":" << e.location.second << " error: " << e.what() << std::endl;
    }catch(Exception::Exception &e){
        std::cerr << path.string() << ": error: " << e.what() << std::endl;
    }

    return 0;
}
