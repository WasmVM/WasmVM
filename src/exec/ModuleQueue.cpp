// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "ModuleQueue.hpp"

#include <decode.hpp>
#include <exception.hpp>

using namespace WasmVM;

ModuleQueue::ModuleQueue(std::filesystem::path main_module, std::vector<std::filesystem::path> extra_paths, std::optional<std::filesystem::path> current_path, std::optional<std::filesystem::path> system_path)
    : current_path(current_path), system_path(system_path){
    
    if(!std::filesystem::exists(main_module)){
        throw Exception::Exception("main module not found");
    }

    for(std::filesystem::path path : extra_paths){
        if(!std::filesystem::exists(path)){
            Exception::Warning(std::string("extra path '" + path.string() + "' not found"));
        }
        this->extra_paths.push_back(path);
    }

    std::stack<std::filesystem::path> path_stack;
    std::filesystem::path current = main_module;

    do{
        
    }while(!path_stack.empty());
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

std::filesystem::path ModuleQueue::resolve(std::string module_name){
    std::filesystem::path module_path(module_name);
    // Exact match
    if(std::filesystem::exists(module_path)){
        return module_path;
    }
    // ./ and ../
    if(module_path.string().starts_with("./") || module_path.string().starts_with("../")){
        throw Exception::Exception(std::string("module '") + module_name + "' not found");
    }
    // extra_paths
    for(std::filesystem::path extra_path : extra_paths){
        auto extended_path = extend(extra_path / module_path);
        if(extended_path){
            return extended_path.value();
        }
    }
    // current_paths
    if(current_path){
        auto extended_path = extend(current_path.value() / module_path);
        if(extended_path){
            return extended_path.value();
        }
    }
    // system_paths
    if(system_path){
        auto extended_path = extend(system_path.value() / module_path);
        if(extended_path){
            return extended_path.value();
        }
    }
    throw Exception::Exception(std::string("module '") + module_name + "' not found");
}