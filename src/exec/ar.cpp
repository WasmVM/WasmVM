// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <fstream>
#include <filesystem>
#include <iostream>
#include <string>
#include <utility>
#include <optional>
#include <regex>

#include <WasmVM.hpp>
#include <exception.hpp>
#include <Util.hpp>
#include <json.hpp>

#include "CommandParser.hpp"
#include "color.hpp"
#include "Archive.hpp"

using namespace WasmVM;

static std::pair<std::filesystem::path, std::optional<std::filesystem::path>> parse_create_path(std::string str){
    static const std::regex path_regex("([^:]*)(:(.*))?");
    std::smatch path_match;
    if(std::regex_match(str, path_match, path_regex)){
        std::pair<std::filesystem::path, std::optional<std::filesystem::path>> result;
        result.first = path_match[1].str();
        if(path_match[2].length() > 0){
            result.second = path_match[3].str();
        }
        return result;
    }else{
        throw Exception::Exception("invalid argument");
    }
}

struct CreateConfig {

    CreateConfig(std::filesystem::path path, std::string prefix) : prefix(prefix) {
        // Load JSON
        std::ifstream config_file(path);
        Json::Value config_json;
        config_file >> config_json;
        config_file.close();
        Json::Value::Object config = config_json.get<Json::Value::Object>();

        // prefix
        if(prefix.empty() && config.contains("prefix")){
            this->prefix = config["prefix"].get<std::string>();
        }

        // paths
        if(config.contains("paths")){
            for(Json::Value value : config["paths"].get<Json::Value::Array>()){
                value.visit(overloaded {
                    [](auto&){
                        throw Exception::Exception("invalid path type");
                    },
                    [&](Json::Value::String& path_str){
                        auto parsed = parse_create_path(path_str);
                        if(!std::filesystem::exists(parsed.first)){
                            Exception::Warning(std::string("file ") + parsed.first.string() + " not found, skipped");
                            return;
                        }
                        std::filesystem::path file_path = std::filesystem::canonical(parsed.first);
                        if(paths.contains(file_path)){
                            Exception::Warning(std::string("multiple definitions found for file ") + parsed.first.string());
                        }
                        paths[file_path] = parsed.second.value_or(std::filesystem::relative(file_path));
                    },
                    [&](Json::Value::Object& path_obj){
                        if(!path_obj.contains("file")){
                            throw Exception::Exception("invalid path object");
                        }
                        std::filesystem::path file_path = path_obj["file"].get<std::string>();
                        if(!std::filesystem::exists(file_path)){
                            Exception::Warning(std::string("file ") + file_path.string() + " not found, skipped");
                            return;
                        }
                        std::filesystem::path canon = std::filesystem::canonical(file_path);
                        if(paths.contains(canon)){
                            Exception::Warning(std::string("multiple definitions found for file ") + file_path.string());
                        }
                        if(path_obj.contains("module")){
                            paths[canon] = path_obj["module"].get<std::string>();
                        }else{
                            paths[canon] = std::filesystem::relative(canon);
                        }
                    }
                });
            }
        }
    }

    std::filesystem::path prefix;
    std::map<std::filesystem::path, std::filesystem::path> paths;
};

int main(int argc, char const *argv[]){  
    // Parse argv
    CommandParser args(argc, argv, {
        CommandParser::Optional("--version", "Show version", "-v"),
        CommandParser::Optional("--prefix", "Path prefix: \n"
            "\t\t\t    create mode: extra prefix for archived module path\n"
            "\t\t\t    extract mode: the folder to extract into (default: $PWD)\n"
            "\t\t\t    list mode: extra prefix prefixed to listed files",
        1, "-p"),
        CommandParser::Optional("--config", "Path of configuration JSON file (create mode only)", 1, "-c"),
        CommandParser::Fixed("mode", "\tOperation mode: \n"
            "\t\t\t    create  (c): create archive\n"
            "\t\t\t    extract (x): extract files from archive\n"
            "\t\t\t    list    (l): list files in the archive"
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

    try {
        // Check required args
        if(!args["mode"]){
            throw Exception::Exception("mode is required");
        }
        if(!args["archive_file"]){
            throw Exception::Exception("no archive file");
        }

        // prefix
        std::filesystem::path prefix;
        if(args["prefix"]){
            prefix = std::get<std::string>(args["prefix"].value());
        }
        // archive file
        Archive archive(std::get<std::string>(args["archive_file"].value()));
        // execute
        std::string mode = std::get<std::string>(args["mode"].value());
        if(mode == "create" || mode == "c"){
            /** Create **/
            CreateConfig config(std::filesystem::path(std::get<std::string>(args["config"].value())), prefix.string());
            if(args["module_files"]){
                std::vector<std::string> module_files = std::get<std::vector<std::string>>(args["module_files"].value());
                for(std::string module_file : module_files){
                    auto parsed = parse_create_path(module_file);
                    if(!std::filesystem::exists(parsed.first)){
                        Exception::Warning(std::string("file ") + parsed.first.string() + " not found, skipped");
                        continue;
                    }
                    std::filesystem::path file_path = std::filesystem::canonical(parsed.first);
                    if(config.paths.contains(file_path)){
                        Exception::Warning(std::string("multiple definitions found for file ") + parsed.first.string());
                    }
                    config.paths[file_path] = parsed.second.value_or(std::filesystem::relative(file_path));
                }
            }
            if(config.paths.empty()){
                throw Exception::Exception("no module files");
            }
            archive.create(config.paths, config.prefix);
        }else if(mode == "extract" || mode == "x"){
            /** Extract **/
            if(!args["module_files"]){
                throw Exception::Exception("no module files");
            }
            std::vector<std::string> module_files = std::get<std::vector<std::string>>(args["module_files"].value());
            for(std::string module_file : module_files){
                if(!archive.extract(std::filesystem::path(module_file).lexically_normal(), prefix).has_value()){
                    Exception::Warning(std::string("module ") + module_file + "not found, skipped");
                }
            }
        }else if(mode == "list" || mode == "l"){
            /** List **/
            for(std::filesystem::path path : archive.list(prefix)){
                std::cout << path << std::endl;
            }
        }else{
            throw Exception::Exception("unknown mode");
        }
    }catch(Exception::Exception &e){
        std::cerr << args.program.filename().string() << ": " COLOR_Error ": " << e.what() << std::endl;
        return -1;
    }

    return 0;
}
