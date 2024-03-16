// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

//***************************************************
// Author	: Jack Lin
// Date 	: 3/14/2014
//***************************************************

#include <fstream>
#include <filesystem>
#include <iostream>
#include <string>

#include <WasmVM.hpp>
#include <exception.hpp>

#include "../CommandParser.hpp"
#include "../color.hpp"
#include "../lib/dump/dump.hpp"
#include "objdump.hpp"


using namespace WasmVM;

int main(int argc, char const *argv[]){  
    // Parse argv
    CommandParser args(argc, argv, {
        CommandParser::Optional("--dump", "dump raw data", "-d"),
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
        std::cerr << "WasmVM assembler version " VERSION << std::endl;
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
    	// mathod



    }catch(Exception::Decode &e){
        std::ios::fmtflags flags = std::cerr.flags();
        std::cerr << input_path.string() << ":" << std::hex << std::showbase << e.location << " " COLOR_Error ": " << e.what() << std::endl;
        std::cerr.setf(flags);
        return -1;
    }catch(Exception::Exception &e){
        std::cerr << input_path.string() << ": " COLOR_Error ": " << e.what() << std::endl;
        return -1;
    }







    return 0;
}