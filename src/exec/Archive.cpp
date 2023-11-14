// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "Archive.hpp"

#include <fstream>
#include <algorithm>
#include <exception.hpp>

#include <iostream>

using namespace WasmVM;

template<typename T>
std::ostream& write_data(std::ostream& os, T value){
    return os.write((char*)&value, sizeof(value));
}

Archive::Archive(std::filesystem::path path) : path(path) {}

struct PathEntry {
    std::filesystem::path path;
    std::streampos address;
    uint64_t module_size;
    uint32_t name_size;
};

bool Archive::check_magic_version(std::istream& stream){
    std::string str(4, '\0');
    stream.read(str.data(), 4);
    if(str != Archive::magic){
        return false;
    }
    stream.read(str.data(), 4);
    if(str[0] != Archive::version[0] || str[1] != Archive::version[1]){
        return false;
    }
    return true;
}

void Archive::create(std::map<std::filesystem::path, std::filesystem::path> modules, std::filesystem::path prefix){
    // Open file
    std::ofstream output(path, std::ios::binary | std::ios::out);

    // Write megic & version
    output.write(magic, sizeof(magic));
    output.write(version, sizeof(version));

    // Write paths section
    std::vector<PathEntry> path_entries;
    auto paths_start = output.tellp();
    write_data(output, (uint64_t) 0); // len(paths): Update later
    write_data(output, (uint32_t) modules.size());
    for(auto module_pair : modules){
        std::string name = (prefix / module_pair.second).lexically_normal().string();
        write_data(output, (uint32_t)name.size()) << name;
        write_data(output, (uint64_t) 0); // address: Update later
        PathEntry& entry = path_entries.emplace_back();
        entry.path = module_pair.first;
        entry.name_size = name.size();
    }
    uint64_t paths_size = output.tellp() - paths_start;

    // Write module
    for(PathEntry& path_entry : path_entries){
        path_entry.address = output.tellp(); // Fill module position
        write_data(output, (uint64_t) 0); // len(module): Update later
        std::filesystem::path module_path = path_entry.path;
        if(!std::filesystem::exists(module_path)){
            throw Exception::Exception(std::string("module '") + module_path.string() + "' not found");
        }
        std::ifstream module_file(module_path, std::ios::in | std::ios::binary);
        char buf[1024];
        uint64_t module_size = 0;
        while(!module_file.eof()){
            module_file.read(buf, 1024);
            std::streamsize extracted = module_file.gcount();
            output.write(buf, extracted);
            module_size += extracted;
        }
        module_file.close();
        path_entry.module_size = module_size; // Fill module size
    }

    // Update paths
    output.seekp(paths_start);
    write_data(output, paths_size); // len(paths)
    output.seekp(sizeof(uint32_t), std::ios::cur); // skip path count
    for(PathEntry& path_entry : path_entries){
        output.seekp(path_entry.name_size + sizeof(uint32_t), std::ios::cur);
        write_data(output, (uint64_t)path_entry.address);
    }

    // Update module length
    for(PathEntry& path_entry : path_entries){
        write_data(output, path_entry.module_size);
        output.seekp(path_entry.module_size, std::ios::cur);
    }

    // Close file
    output.close();
}

std::optional<std::filesystem::path> Archive::extract(std::filesystem::path module, std::filesystem::path prefix){
    std::optional<std::filesystem::path> output_path = (prefix / module).lexically_normal();
    // Open file
    std::ifstream input(path, std::ios::binary | std::ios::in);
    if(!check_magic_version(input)){
        throw Exception::Exception("incorrect magic or version");
    }
    input.seekg(sizeof(uint64_t), std::ios::cur); // Skip paths length
    // Read path
    uint32_t path_count;
    uint64_t address = 0;
    input.read((char*)&path_count, sizeof(uint32_t));
    for(uint32_t index = 0; index < path_count; ++index){
        uint32_t name_length;
        input.read((char*)&name_length, sizeof(uint32_t));
        std::string name(name_length, '\0');
        input.read(name.data(), name_length);
        if(name == module){
            input.read((char*)&address, sizeof(uint64_t));
            break;
        }else{
            input.seekg(sizeof(uint64_t), std::ios::cur); 
        }
    }
    // Read content
    if(address != 0){
        // Read module size
        input.seekg(address, std::ios::beg);
        uint64_t module_size = 0;
        input.read((char*)&module_size, sizeof(uint64_t));
        // Create parent path
        std::filesystem::create_directories(output_path.value().parent_path());
        // Extract content
        std::ofstream output(output_path.value(), std::ios::binary | std::ios::out);
        char buf[1024];
        while(module_size > 0){
            input.read(buf, std::min(module_size, (uint64_t)1024));
            std::streamsize extracted = input.gcount();
            output.write(buf, extracted);
            module_size -= extracted;
        }
        // Close output
        output.close();
    }else{
        output_path.reset();
    }
    // Close input
    input.close();
    return output_path;
}

std::vector<std::filesystem::path> Archive::list(std::filesystem::path prefix){
    std::vector<std::filesystem::path> result;
    // Open file
    std::ifstream input(path, std::ios::binary | std::ios::in);
    if(!check_magic_version(input)){
        throw Exception::Exception("incorrect magic or version");
    }
    input.seekg(sizeof(uint64_t), std::ios::cur); // Skip paths length
    // Read path
    uint32_t path_count;
    input.read((char*)&path_count, sizeof(uint32_t));
    for(uint32_t index = 0; index < path_count; ++index){
        uint32_t name_length;
        input.read((char*)&name_length, sizeof(uint32_t));
        std::string name(name_length, '\0');
        input.read(name.data(), name_length);
        result.push_back((prefix / name).lexically_normal());
        input.seekg(sizeof(uint64_t), std::ios::cur); 
    }

    // Close file
    input.close();
    return result;
}