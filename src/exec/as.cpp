// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <fstream>
#include <filesystem>
#include <iostream>
#include <string>

#include <WasmVM.hpp>
#include <exception.hpp>

#include "CommandParser.hpp"
#include "color.hpp"

using namespace WasmVM;

int main(int argc, char const *argv[]){  
    // Parse argv
    CommandParser args(argc, argv, {
        CommandParser::Optional {"--version", "Show version", .alias = "-v"},
        CommandParser::Optional {"--force", "Skip validation", .alias = "-f"},
        CommandParser::Optional {"--output", "Output file name", 1, .alias = "-o"},
        CommandParser::Fixed {"input_file", "Input file in WebAssembly text format"},
    },
        "wasmvm-as : WasmVM assembler"
    );
    // Warnings
    Exception::Warning::regist([](std::string message){
        std::cerr << COLOR_Warning ": " << message << std::endl;
    });
    // version
    if(args["version"]){
        std::cerr << "WasmVM assembler version " VERSION << std::endl;
        return 0;
    }
    // input file
    if(!args["input_file"]){
        std::cerr << "wasmvm-as : " COLOR_Error ": no input file" << std::endl;
        return -1;
    }
    std::filesystem::path input_path(std::get<std::string>(args["input_file"].value()));
    std::ifstream input_file(input_path);
    size_t input_size = std::filesystem::file_size(input_path);
    std::string src(input_size, ' ');
    input_file.read(src.data(), input_size);
    input_file.close();

    try {
        WasmModule result = module_parse(src);
        
    }catch(Exception::Parse &e){
        std::cerr << input_path.string() << ":" << e.location.first << ":" << e.location.second << " " COLOR_Error ": " << e.what() << std::endl;
    }catch(Exception::Exception &e){
        std::cerr << input_path.string() << ": " COLOR_Error ": " << e.what() << std::endl;
    }

    return 0;
}
