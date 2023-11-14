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
#include <utility>
#include <set>

#include <WasmVM.hpp>
#include <json.hpp>
#include <exception.hpp>
#include <Util.hpp>

#include "CommandParser.hpp"
#include "color.hpp"
#include "Linker.hpp"
#include "Archive.hpp"

using namespace WasmVM;

static void consume_archive(std::filesystem::path archive_path, Linker& linker){
    // Open archive
    std::ifstream archive(archive_path, std::ios::binary | std::ios::in);
    if(!Archive::check_magic_version(archive)){
        throw Exception::Exception("invalid archive version");
    }
    // Read paths
    std::vector<std::pair<std::filesystem::path, uint64_t>> paths;
    archive.seekg(sizeof(uint64_t), std::ios::seekdir::cur);
    uint32_t path_count;
    archive.read((char*)&path_count, sizeof(uint32_t));
    for(uint32_t index = 0; index < path_count; ++index){
        uint32_t name_length;
        archive.read((char*)&name_length, sizeof(uint32_t));
        std::string name(name_length, '\0');
        archive.read(name.data(), name_length);
        uint64_t address;
        archive.read((char*)&address, sizeof(uint64_t));
        paths.emplace_back(name, address);
    }
    // Close archive
    archive.close();

    // Read contents
    for(auto path_pair : paths){
        // Open module
        std::ifstream module_binary(archive_path, std::ios::binary | std::ios::in);
        module_binary.seekg(path_pair.second, std::ios::seekdir::beg);
        uint64_t module_length;
        module_binary.read((char*)&module_length, sizeof(uint64_t));
        auto module_start = module_binary.tellg();
        linker.consume(path_pair.first, module_binary);
        if((module_binary.tellg() - module_start) != module_length){
            Exception::Warning(path_pair.first.string() + " module length may not match expected length");
        }
        // Close module
        module_binary.close();
    }
}

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
        if(config_obj.contains("modules")){
            for(auto& elem : config_obj["modules"].get<Json::Value::Array>()){
                module_paths.emplace_back(std::filesystem::path(elem.get<std::string>()).lexically_normal());
            }
        }
        if(args["modules"]){
            std::vector<std::string> mods = std::get<std::vector<std::string>>(args["modules"].value());
            for(std::string mod : mods){
                module_paths.emplace_back(std::filesystem::path(mod).lexically_normal());
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
                                start_config.emplace_back(start_obj["path"].get<std::string>(), index);
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
                WasmExport& export_ = config.explicit_exports[export_match[1].str()].emplace_back();
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
                                WasmExport& export_ = config.explicit_exports[export_module].emplace_back();
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
        std::set<std::filesystem::path> path_set;
        for(std::filesystem::path module_path : module_paths){
            if(!std::filesystem::exists(module_path)){
                throw Exception::Exception(module_path.string() + " not exist");
            }
            module_path = std::filesystem::canonical(module_path);
            if(!path_set.contains(module_path)){
                // Check magic
                std::ifstream fin(module_path, std::ios::binary | std::ios::in);
                std::string magic(4, '\0');
                fin.read(magic.data(), 4);
                fin.close();
                if(magic == Archive::magic){
                    // static lib
                    consume_archive(module_path, linker);
                }else if(*((uint32_t*)magic.data()) == 0x6d736100u){
                    // wasm module
                    std::ifstream module_file(module_path, std::ios::binary | std::ios::in);
                    linker.consume(module_path, module_file);
                    module_file.close();
                }else{
                    throw Exception::Exception(std::string("unknown binary file '") + module_path.filename().string() + "'");
                }
                path_set.emplace(module_path);
            }
        }

        // Encode
        std::ofstream output_file(output_path, std::ios::binary | std::ios::trunc);
        module_encode(linker.get(), output_file);
        output_file.close();
        
    }catch(Exception::Exception &e){
        std::cerr << args.program.filename().string() << ": " COLOR_Error ": " << e.what() << std::endl;
        return -1;
    }

    return 0;
}