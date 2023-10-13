// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <algorithm>

#include <WasmVM.hpp>
#include <exception.hpp>

#include "CommandParser.hpp"
#include "color.hpp"
#include "Linker.hpp"

using namespace WasmVM;

/* TODO: Support linker config:
 * exports
 * start function: explicit, merge, concat
 * explicit imports
 */

int main(int argc, char const *argv[]){
    // Parse argv
    CommandParser args(argc, argv, {
        CommandParser::Optional("--version", "Show version", "-v"),
        CommandParser::Fixed("output", "File name of output module"),
        CommandParser::Fixed("modules", "Path of modules", (unsigned int)index_npos)
    },
        "wasmvm-ld : WasmVM linker"
    );

    // Warnings
    Exception::Warning::regist([](std::string message){
        std::cerr << COLOR_Warning ": " << message << std::endl;
    });
    // version
    if(args["version"]){
        std::cerr << "WasmVM linker version " VERSION << std::endl;
        return 0;
    }

    try {
        // Output path
        if(!args["output"] || !args["modules"]){
            throw Exception::Exception("no module");
        }
        std::filesystem::path output_path(std::get<std::string>(args["output"].value()));

        // Get paths
        std::vector<std::filesystem::path> module_paths;
        {
            std::set<std::filesystem::path> path_set;
            std::vector<std::string> paths = std::get<std::vector<std::string>>(args["modules"].value());
            for(std::string path_str : paths){
                std::filesystem::path canon = std::filesystem::canonical(path_str);
                if(!path_set.contains(canon)){
                    path_set.insert(canon);
                    module_paths.emplace_back(canon);
                }
            }
        }

        // Link
        Linker linker;
        for(std::filesystem::path module_path : module_paths){
            // TODO: support static lib
            linker.consume(module_path);
        }

        // Encode
        std::ofstream output_file(output_path, std::ios::binary | std::ios::trunc);
        module_encode(linker.get(), output_file);
        output_file.close();
        
    }catch(Exception::Exception &e){
        std::cerr << std::filesystem::path(argv[0]).filename().string() << ": " COLOR_Error ": " << e.what() << std::endl;
        return -1;
    }

    return 0;
}