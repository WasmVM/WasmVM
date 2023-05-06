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
        CommandParser::Optional("--version", "Show version", "-v"),
        CommandParser::Optional("--force", "Skip validation", "-f"),
        CommandParser::Optional("--type", "Show types", "-t"),
        CommandParser::Optional("--import", "Show imports", "-i"),
        CommandParser::Optional("--func", "Show functions", "-f"),
        CommandParser::Optional("--table", "Show tables", "-b"),
        CommandParser::Optional("--memory", "Show memories", "-m"),
        CommandParser::Optional("--global", "Show globlas", "-g"),
        CommandParser::Optional("--export", "Show exports", "-x"),
        CommandParser::Optional("--start", "Show start", "-s"),
        CommandParser::Optional("--elem", "Show elements", "-e"),
        CommandParser::Optional("--data", "Show datas", "-d"),
        CommandParser::Fixed("input_file", "Input file in WebAssembly binary format"),
    },
        "readwasm : WasmVM wasm read/disassembler"
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
        std::cerr << "readwasm : " COLOR_Error ": no input file" << std::endl;
        return -1;
    }
    // Input path
    std::filesystem::path input_path(std::get<std::string>(args["input_file"].value()));
    if(!std::filesystem::exists(input_path)){
        std::cerr << "readwasm : " COLOR_Error ": no such file '" << input_path.string() << "'" << std::endl;
        return -1;
    }

    try {
        // Decode
        std::ifstream input_file(input_path);
        WasmModule decoded_module = module_decode(input_file);
        input_file.close();
        // Dump
    }catch(Exception::Exception &e){
        std::cerr << input_path.string() << ": " COLOR_Error ": " << e.what() << std::endl;
    }

    return 0;
}
