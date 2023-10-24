// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <regex>
#include <algorithm>

#include <WasmVM.hpp>
#include <exception.hpp>

#include "CommandParser.hpp"
#include "color.hpp"
#include "Linker.hpp"

using namespace WasmVM;

/* TODO: Support linker config:
 * exports
 * start function: explicit (module:func_index), compose
 * explicit imports (module:name)
 */

int main(int argc, char const *argv[]){
    // Parse argv
    CommandParser args(argc, argv, {
        CommandParser::Optional("--version", "Show version", "-v"),
        CommandParser::Optional("--start", "Specify start function", 1, "-s"),
        CommandParser::Optional("--exports", "Specify exports function", 1, "-e"),
        CommandParser::Optional("--imports", "Specify explicit imports", 1, "-i"),
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

        // Config
        Linker::Config config;
        // Start
        if(args["start"]){
            std::string start_str = std::get<std::string>(args["start"].value());
            if(start_str == "compose"){
                config.start_func.emplace<Linker::Config::StartMode>(Linker::Config::Compose);
            }else{
                const std::regex start_regex("(.*):(\\d+)");
                std::smatch start_match;
                if(std::regex_match(start_str, start_match, start_regex)){
                    config.start_func.emplace<std::pair<std::filesystem::path, index_t>>(
                        std::filesystem::canonical(start_match[1].str()),
                        std::stoi(start_match[2].str())
                    );
                }else{
                    throw Exception::Exception("invalid start function config");
                }
            }
        }
        // Imports
        if(args["imports"]){
            std::string import_str = std::get<std::string>(args["imports"].value());
            const std::regex import_regex("^([^,:]*):([^,]*)(,|$)");
            std::smatch import_match;
            while(std::regex_search(import_str, import_match, import_regex)){
                config.explicit_imports[import_match[1].str()].emplace(import_match[2].str());
                import_str = import_str.substr(import_match[0].length());
            }
            if(!import_str.empty()){
                throw Exception::Exception("invalid explicit import config");
            }
        }

        // Link
        Linker linker(config);
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