// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <algorithm>
#include <functional>

#include <WasmVM.hpp>
#include <json.hpp>
#include <exception.hpp>
#include <Util.hpp>

#include "CommandParser.hpp"
#include "color.hpp"
#include "Linker.hpp"

using namespace WasmVM;

/* TODO: Support linker config:
 * exports (module:name:desc:index)
 * explicit imports (module:name)
 */

int main(int argc, char const *argv[]){
    // Parse argv
    CommandParser args(argc, argv, {
        CommandParser::Optional("--version", "Show version", "-v"),
        CommandParser::Optional("--config", "Specify config JSON file", 1, "-c"),
        CommandParser::Optional("--start", "Specify start function.\t\nFormat: all | <module>:<func_index>[,<module>:<func_index>]...", 1, "-s"),
        CommandParser::Optional("--exports", "Specify explicit exports", 1, "-e"),
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
        // Config
        Linker::Config config;
        Json::Value::Object config_obj;

        if(args["config"]){
            std::ifstream config_file(std::get<std::string>(args["config"].value()));
            Json::Value config_json;
            config_file >> config_json;
            config_file.close();
            config_obj = config_json.get<Json::Value::Object>();
        }
        // Output path
        std::filesystem::path output_path;
        if(args["output"] || config_obj.contains("output")){
            output_path = args["output"] ? std::get<std::string>(args["output"].value()) : config_obj["output"].get<std::string>();
        }else{
            throw Exception::Exception("no output");
        }

        // Get paths
        std::vector<std::filesystem::path> module_paths;
        {
            std::set<std::filesystem::path> path_set;
            std::vector<std::string> paths;
            if(config_obj.contains("modules")){
                for(auto& elem : config_obj["modules"].get<Json::Value::Array>()){
                    paths.emplace_back(elem.get<std::string>());
                }
            }
            if(args["modules"]){
                std::vector<std::string> mods = std::get<std::vector<std::string>>(args["modules"].value());
                for(std::string mod : mods){
                    paths.emplace_back(mod);
                }
            }
            for(std::string path_str : paths){
                std::filesystem::path canon = std::filesystem::canonical(path_str);
                if(!path_set.contains(canon)){
                    path_set.insert(canon);
                    module_paths.emplace_back(canon);
                }
            }
        }
        if(module_paths.empty()){
            throw Exception::Exception("no modules");
        }

        // Start
        std::function<std::vector<Linker::Config::StartEntry>(std::string)> get_start_pair = [](std::string start_str){
            std::vector<Linker::Config::StartEntry> results;
            std::set<std::filesystem::path> paths;
            static const std::regex start_regex("^([^,:]*)(:(\\d+))?(,|$)");
            std::smatch start_match;
            while(std::regex_search(start_str, start_match, start_regex) && !start_str.empty()){
                std::filesystem::path module_path = std::filesystem::canonical(start_match[1].str());
                if(paths.contains(module_path)){
                    throw Exception::Exception("multiple start functions are invalid");
                }
                std::optional<index_t> index;
                if(start_match[2].length() > 0){
                    index.emplace(std::stoi(start_match[3].str()));
                }
                results.emplace_back(module_path, index);
                start_str = start_str.substr(start_match[0].length());
            }
            if(!start_str.empty()){
                throw Exception::Exception("invalid start function config");
            }
            return results;
        };
        if(args["start"]){
            std::string start_str = std::get<std::string>(args["start"].value());
            if(start_str == "all"){
                config.start_func.emplace<Linker::Config::StartMode>(Linker::Config::All);
            }else{
                config.start_func.emplace<std::vector<Linker::Config::StartEntry>>(get_start_pair(start_str));
            }
        }else if(config_obj.contains("start")){
            config_obj["start"].visit(overloaded {
                [](auto&){
                    throw Exception::Exception("invalid start function config");
                },
                [&](Json::Value::String& start_str){
                    if(start_str == "all"){
                        config.start_func.emplace<Linker::Config::StartMode>(Linker::Config::All);
                    }else{
                        config.start_func.emplace<std::vector<Linker::Config::StartEntry>>(get_start_pair(start_str));
                    }
                },
                [&](Json::Value::Array& start_arr){
                    auto& start_config = config.start_func.emplace<std::vector<Linker::Config::StartEntry>>();
                    for(Json::Value entry : start_arr){
                        entry.visit(overloaded {
                            [&](auto&){
                                throw Exception::Exception("invalid start function config element");
                            },
                            [&](Json::Value::String& start_str){
                                std::vector<Linker::Config::StartEntry> entries = get_start_pair(start_str);
                                if(entries.size() != 1){
                                    throw Exception::Exception("invalid start function config element");
                                }
                                start_config.emplace_back(entries[0]);
                            },
                            [&](Json::Value::Object& start_obj){
                                std::optional<index_t> index;
                                if(start_obj.contains("index")){
                                    index.emplace((index_t)start_obj["index"].get<Json::Value::Number>());
                                }
                                start_config.emplace_back(std::filesystem::canonical(start_obj["path"].get<std::string>()), index);
                            }
                        });
                    }
                }
            });
        }else{
            config.start_func.emplace<Linker::Config::StartMode>(Linker::Config::None);
        }

        // Imports
        std::function<void(std::string)> expand_imports = [&](std::string import_str){
            const std::regex import_regex("^([^,:]*):([^,]*)(,|$)");
            std::smatch import_match;
            while(std::regex_search(import_str, import_match, import_regex)){
                config.explicit_imports[import_match[1].str()].emplace(import_match[2].str());
                import_str = import_str.substr(import_match[0].length());
            }
            if(!import_str.empty()){
                throw Exception::Exception("invalid explicit import config");
            }
        };
        if(args["imports"]){
            std::string import_str = std::get<std::string>(args["imports"].value());
            expand_imports(import_str);
        }else if(config_obj.contains("imports")){
            config_obj["imports"].visit(overloaded {
                [](auto&){
                    throw Exception::Exception("invalid explicit imports config");
                },
                [&](Json::Value::String& import_str){
                    expand_imports(import_str);
                },
                [&](Json::Value::Object& imports_obj){
                    for(auto& entry : imports_obj){
                        Json::Value::Array imports_names = entry.second.get<Json::Value::Array>();
                        for(Json::Value& name : imports_names){
                            config.explicit_imports[entry.first].emplace(name.get<std::string>());
                        }
                    }
                }
            });
        }

        // Exports
        std::function<void(std::string)> expand_exports = [&](std::string export_str){
            const std::regex export_regex("^([^,:]*):([^,:]*):([^,:]*):([^,]*)(,|$)");
            std::smatch export_match;
            while(std::regex_search(export_str, export_match, export_regex)){
                std::string export_module = export_match[1].str();
                std::filesystem::path export_path;
                // Extend path
                if(export_module.starts_with("/")){
                    export_path = export_module;
                }else{
                    export_path = std::filesystem::current_path() / export_module;
                }
                // Check existance
                if(!std::filesystem::exists(export_path)){
                    export_path.replace_extension(".wasm");
                }
                if(std::filesystem::exists(export_path)){
                    export_path = std::filesystem::canonical(export_path);
                }else{
                    throw Exception::Exception(std::string("export module '" ) + export_module + "' not found");
                }
                WasmExport& export_ = config.explicit_exports[export_path.string()].emplace_back();
                export_.name = export_match[2].str();
                std::string desc_type = export_match[3].str();
                if(desc_type == "func"){
                    export_.desc = WasmExport::DescType::func;
                }else if(desc_type == "table"){
                    export_.desc = WasmExport::DescType::table;
                }else if(desc_type == "mem"){
                    export_.desc = WasmExport::DescType::mem;
                }else if(desc_type == "global"){
                    export_.desc = WasmExport::DescType::global;
                }else{
                    throw Exception::Exception("invalid explicit export type");
                }
                export_.index = std::stoi(export_match[4].str());
                export_str = export_str.substr(export_match[0].length());
            }
            if(!export_str.empty()){
                throw Exception::Exception("invalid explicit export config");
            }
        };
        if(args["exports"]){
            std::string export_str = std::get<std::string>(args["exports"].value());
            expand_exports(export_str);
        }else if(config_obj.contains("exports")){
            config_obj["exports"].visit(overloaded {
                [](auto&){
                    throw Exception::Exception("invalid explicit export config");
                },
                [&](Json::Value::String& export_str){
                    expand_exports(export_str);
                },
                [&](Json::Value::Array& export_arr){
                    for(Json::Value& entry : export_arr){
                        entry.visit(overloaded{
                            [](auto&){
                                throw Exception::Exception("invalid explicit export config");
                            },
                            [&](Json::Value::String& entry_str){
                                expand_exports(entry_str);
                            },
                            [&](Json::Value::Object& entry_obj){
                                std::string export_module = entry_obj["path"].get<std::string>();
                                std::filesystem::path export_path;
                                // Extend path
                                if(export_module.starts_with("/")){
                                    export_path = export_module;
                                }else{
                                    export_path = std::filesystem::current_path() / export_module;
                                }
                                // Check existance
                                if(!std::filesystem::exists(export_path)){
                                    export_path.replace_extension(".wasm");
                                }
                                if(std::filesystem::exists(export_path)){
                                    export_path = std::filesystem::canonical(export_path);
                                }else{
                                    throw Exception::Exception(std::string("export module '" ) + export_module + "' not found");
                                }
                                WasmExport& export_ = config.explicit_exports[export_path.string()].emplace_back();
                                export_.name = entry_obj["name"].get<std::string>();
                                std::string desc_type = entry_obj["type"].get<std::string>();
                                if(desc_type == "func"){
                                    export_.desc = WasmExport::DescType::func;
                                }else if(desc_type == "table"){
                                    export_.desc = WasmExport::DescType::table;
                                }else if(desc_type == "mem"){
                                    export_.desc = WasmExport::DescType::mem;
                                }else if(desc_type == "global"){
                                    export_.desc = WasmExport::DescType::global;
                                }else{
                                    throw Exception::Exception("invalid explicit export type");
                                }
                                export_.index = (index_t)entry_obj["index"].get<Json::Value::Number>();
                            },
                        });
                    }
                }
            });
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