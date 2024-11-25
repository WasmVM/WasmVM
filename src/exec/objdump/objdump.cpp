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
        CommandParser::Optional("--dump_version", "Show version", "-dv"),


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
        std::ifstream st(input_path, std::ios::binary);
        Stream stream(st);
    	
        if(args["dump_raw"]){
            std::cout << "Address: byte" << std::endl;
            std::cout << stream;
            st.close();
            return 0;
        }

        Bytes magic(4);
        stream >> magic;
        stream.print_address(magic);
        std::cout << magic << " ; WASM_BINARY_MAGIC" << std::endl;

        Bytes version(4);
        stream >> version;
        stream.print_address(version);
        std::cout << version << " ; WASM_BINARY_VERSION" << std::endl;

        if(stream.peek_section(stream) == 0x01){
            std::cout << ";; type section" << std::endl;
            TypeSection funcs(stream);
            stream >> funcs;
            std::cout << funcs;
        }
        
        if(stream.peek_section(stream) == 0x02){
            std::cout << ";; import section" << std::endl;
            ImportSection import(stream);
            stream >> import;
            std::cout << import;
        }
        
        if(stream.peek_section(stream) == 0x03){
            std::cout << ";; function section" << std::endl;
            FunctionSection function(stream);
            stream >> function;
            std::cout << function;
        }


        st.close();

    }catch(Exception::Exception &e){
        std::cerr << input_path.string() << ": " COLOR_Error ": " << e.what() << std::endl;
        return -1;
    }

    return 0;
}