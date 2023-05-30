// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <fstream>
#include <filesystem>
#include <iostream>
#include <string>
#include <variant>

#include <WasmVM.hpp>
#include <exception.hpp>

#include "CommandParser.hpp"
#include "ModuleQueue.hpp"
#include "color.hpp"

using namespace WasmVM;

int main(int argc, char const *argv[]){  
    // Parse argv
    CommandParser args(argc, argv, {
        CommandParser::Optional("--version", "Show version", "-v"),
        CommandParser::Optional("--no-system", "Disable modules from system module path", "-ns"),
        CommandParser::Optional("--no-current", "Disable modules from current path", "-nc"),
        CommandParser::Fixed("main_module", "main WebAssembly module binary"),
        CommandParser::Fixed("extra_path", "Path to find modules", (unsigned int)index_npos)
    },
        "wasmvm : WasmVM WebAssembly virtual machine"
    );
    // Warnings
    Exception::Warning::regist([](std::string message){
        std::cerr << COLOR_Warning ": " << message << std::endl;
    });
    // version
    if(args["version"]){
        std::cerr << "WasmVM version " VERSION << std::endl;
        return 0;
    }

    try {
        // Specify paths
        std::vector<std::filesystem::path> extra_paths;
        std::optional<std::filesystem::path> current_path;
        std::optional<std::filesystem::path> system_path;
        if(args["extra_path"]){
            std::vector<std::string> paths = std::get<std::vector<std::string>>(args["extra_path"].value());
            for(std::string path : paths){
                extra_paths.emplace_back(path);
            }
        }
        if(!args["main_module"]){
            throw Exception::Exception("no main module");
        }
        if(!args["no-current"]){
            current_path = std::filesystem::current_path();
        }
        if(!args["no-sysyem"]){
            system_path = DEFAULT_MODULE_PATH;
        }

        // Get module queue
        std::string main_module = std::get<std::string>(args["main_module"].value());
        ModuleQueue module_queue(main_module, extra_paths, current_path, system_path);

        
    }catch(Exception::Exception &e){
        std::cerr << std::filesystem::path(argv[0]).filename().string() << ": " COLOR_Error ": " << e.what() << std::endl;
        return -1;
    }
    return 0;
}
