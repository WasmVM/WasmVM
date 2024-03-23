// Copyright 2024 Luis Hsu, Jack Lin. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <fstream>
#include <filesystem>
#include <iostream>
#include <string>

#include <WasmVM.hpp>
#include <exception.hpp>

#include "../CommandParser.hpp"
#include "../color.hpp"
#include "objdump.hpp"

using namespace WasmVM;
using namespace Objdump;

int main(int argc, char const *argv[]){  
    // Parse argv
    CommandParser args(argc, argv, {
        CommandParser::Optional("--dump_raw", "dump raw data", "-d"),
        CommandParser::Optional("--dump_1", "dump raw date for one bytes", "-d1"),
        CommandParser::Optional("--dump_2", "dump raw date for two bytes", "-d2"),
        CommandParser::Optional("--dump_3", "dump raw date for three bytes", "-d3"),
        CommandParser::Optional("--version", "Show version", "-v"),
        // CommandParser::Optional("--force", "Skip validation", "-f"),
        CommandParser::Fixed("input_file", "Input file in WebAssembly binary format"),
    },
        "wasmdump : WasmVM dump wasm"
    );
    // Warnings
    Exception::Warning::regist([](std::string message){
        std::cerr << COLOR_Warning ": " << message << std::endl;
    });
    // version
    if(args["version"]){
        std::cerr << "Wasm dump version " VERSION << std::endl;
        return 0;
    }
    // input file
    if(!args["input_file"]){
        std::cerr << "wasmdump : " COLOR_Error ": no input file" << std::endl;
        return -1;
    }
    // Input path
    std::filesystem::path input_path(std::get<std::string>(args["input_file"].value()));
    if(!std::filesystem::exists(input_path)){
        std::cerr << "wasmdump : " COLOR_Error ": no such file '" << input_path.string() << "'" << std::endl;
        return -1;
    }


    try {
        if(!args["dump_raw"] && !args["dump_magic"]){
            std::cout << "Dear, you forget input argument." << std::endl;
        }
        // Validate
        // if(!args["force"]){
        //     auto validate_result = module_validate(decoded_module);
        //     if(validate_result){
        //         throw validate_result.value();
        //     }
        // }


        std::ifstream st(input_path, std::ios::binary);
        Stream stream(st);
    	
        if(args["dump_raw"]){
            std::cout << "Address: byte" << std::endl;
            std::cout << stream;
        }

        if(args["dump_1"]){
            std::cout << "Address: byte" << std::endl;
            printBytes(1);
        }
        if(args["dump_2"]){
            std::cout << "Address: byte" << std::endl;
            printBytes(2);
        }
        if(args["dump_3"]){
            std::cout << "Address: byte" << std::endl;
            printBytes(3);
        }

        // if(args["dump_magic"]){
        //     std::cout << "WASM_BINARY_MAGIC, WASM_BINARY_VERSION" << std::endl;
        //     std::cout << "Address: byte" << std::endl;
        //     // TODO print MAGIC and VERSION raw
        //     std::cout << stream;
        // }

        st.close();
    	

    }catch(Exception::Exception &e){
        std::cerr << input_path.string() << ": " COLOR_Error ": " << e.what() << std::endl;
        return -1;
    }





    return 0;
}