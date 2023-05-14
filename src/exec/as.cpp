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
        CommandParser::Optional("--output", "Output file name", 1, "-o"),
        CommandParser::Fixed("input_file", "Input file in WebAssembly text format"),
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
    // Input path
    std::filesystem::path input_path(std::get<std::string>(args["input_file"].value()));
    if(!std::filesystem::exists(input_path)){
        std::cerr << "wasmvm-as : " COLOR_Error ": no such file '" << input_path.string() << "'" << std::endl;
        return -1;
    }
    // Output path
    std::filesystem::path output_path(input_path);
    if(args["output"]){
        output_path = std::get<std::string>(args["output"].value());
    }else{
        output_path.replace_extension("wasm");
    }

    try {
        // Parse
        std::ifstream input_file(input_path);
        WasmModule parsed_module = module_parse(input_file);
        input_file.close();

        // Validate
        if(!args["force"]){
            auto validate_result = module_validate(parsed_module);
            if(validate_result){
                throw validate_result.value();
            }
        }

        // Encode
        std::ofstream output_file(output_path, std::ios::binary | std::ios::trunc);
        module_encode(parsed_module, output_file);
        output_file.close();
        
    }catch(Exception::Parse& e){
        std::cerr << input_path.string() << ":" << e.location.first << ":" << e.location.second << " " COLOR_Error ": " << e.what() << std::endl;
        return -1;
    }catch(Exception::Exception& e){
        std::cerr << input_path.string() << ": " COLOR_Error ": " << e.what() << std::endl;
        return -1;
    }

    return 0;
}
