// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "Archive.hpp"

#include <fstream>
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
    output.seekp(sizeof(uint32_t), std::ios::seekdir::cur); // skip path count
    for(PathEntry& path_entry : path_entries){
        output.seekp(path_entry.name_size + sizeof(uint32_t), std::ios::seekdir::cur);
        write_data(output, (uint64_t)path_entry.address);
    }

    // Update module length
    for(PathEntry& path_entry : path_entries){
        write_data(output, path_entry.module_size);
        output.seekp(path_entry.module_size, std::ios::seekdir::cur);
    }

    // Close file
    output.close();
}

std::filesystem::path Archive::extract(std::string module, std::filesystem::path prefix){

}

std::vector<std::filesystem::path> Archive::list(std::filesystem::path prefix){

}