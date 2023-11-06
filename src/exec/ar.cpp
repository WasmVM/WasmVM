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
        CommandParser::Fixed("mode", "\tOperation mode: \n"
            "\t\t\t    create  (c): create archive\n"
            "\t\t\t    extract (x): extract files from archive\n"
            "\t\t\t    list    (l): list files in the archive\n"
        ),
        CommandParser::Fixed("archive_file", "Path or file name of archive file"),
        CommandParser::Fixed("module_files", "Path of file name of module (create/extract mode only)", (unsigned int)index_npos)
    },
        "wasmvm-ar : WasmVM archiver"
    );
    // Warnings
    Exception::Warning::regist([](std::string message){
        std::cerr << COLOR_Warning ": " << message << std::endl;
    });
    // version
    if(args["version"]){
        std::cerr << "WasmVM archiver version " VERSION << std::endl;
        return 0;
    }
    // mode
    

    return 0;
}
