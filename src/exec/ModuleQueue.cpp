// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "ModuleQueue.hpp"

#include <decode.hpp>
#include <exception.hpp>

#include <fstream>
#include <set>
#include <functional>

using namespace WasmVM;

ModuleQueue::ModuleQueue(
    std::filesystem::path main_module,
    std::map<std::filesystem::path, ModuleInst>& module_insts,
    std::vector<std::filesystem::path> extra_paths,
    std::optional<std::filesystem::path> system_path,
    bool check_parent,
    bool validate
) : module_insts(module_insts), system_path(system_path), check_parent(check_parent){
    
    if(!std::filesystem::exists(main_module)){
        throw Exception::Exception("main module not found");
    }

    for(std::filesystem::path path : extra_paths){
        if(!std::filesystem::exists(path)){
            Exception::Warning(std::string("extra path '" + path.string() + "' not found"));
        }
        this->extra_paths.push_back(path);
    }

    std::set<std::filesystem::path> ancestors;
    std::set<std::filesystem::path> members;
    std::function<void(std::filesystem::path)> find_deps = [&](std::filesystem::path file_path){
        Node node;
        node.file_path = file_path;
        std::ifstream istream(file_path);
        node.module = module_decode(istream);
        if(validate){
            std::optional<Exception::Exception> err = module_validate(node.module);
            if(err){
                throw err.value();
            }
        }
        istream.close();
        if(!node.module.imports.empty()){
            ancestors.emplace(file_path);
            for(WasmImport& import : node.module.imports){
                if(module_insts.contains(import.module)){
                    node.import_paths.emplace_back(import.module);
                    continue;
                }
                std::filesystem::path import_path = std::filesystem::canonical(resolve(import.module, file_path.parent_path()));
                if(ancestors.contains(import_path)){
                    throw Exception::Exception(std::string("invalid loop dependency of '") + import.module + "'");
                }
                node.import_paths.emplace_back(import_path);
                find_deps(import_path);
            }
            ancestors.erase(file_path);
        }
        if(!members.contains(file_path)){
            modules.emplace(node);
            members.emplace(file_path);
        }
    };
    find_deps(std::filesystem::canonical(main_module));
}

ModuleQueue::Node ModuleQueue::pop(){
    ModuleQueue::Node res = modules.front();
    modules.pop();
    return res;
}

static std::optional<std::filesystem::path> extend(std::filesystem::path module_path){
    if(std::filesystem::exists(module_path)){
        return module_path;
    }
    // .wasm
    module_path.replace_extension(module_path.extension().string() + ".wasm");
    if(std::filesystem::exists(module_path)){
        return module_path;
    }
    return std::nullopt;
}

std::filesystem::path ModuleQueue::resolve(std::string module_name, std::filesystem::path parent){
    // Exact match
    if(std::filesystem::exists(module_name)){
        return module_name;
    }
    // ./ and ../
    if(module_name.starts_with("./") || module_name.starts_with("../")){
        auto extended_path = extend(parent / module_name);
        if(extended_path){
            return extended_path.value();
        }
        throw Exception::Exception(std::string("module '") + module_name + "' not found");
    }
    // extra_paths
    for(std::filesystem::path extra_path : extra_paths){
        auto extended_path = extend(extra_path / module_name);
        if(extended_path){
            return extended_path.value();
        }
    }
    // current_paths
    if(check_parent){
        auto extended_path = extend(parent / module_name);
        if(extended_path){
            return extended_path.value();
        }
    }
    // system_paths
    if(system_path){
        auto extended_path = extend(system_path.value() / module_name);
        if(extended_path){
            return extended_path.value();
        }
    }
    throw Exception::Exception(std::string("module '") + module_name + "' not found");
}