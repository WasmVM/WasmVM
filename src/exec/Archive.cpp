// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "Archive.hpp"

using namespace WasmVM;

Archive::Archive(std::filesystem::path path) : path(path) {}

void Archive::create(std::map<std::filesystem::path, std::filesystem::path> modules, std::filesystem::path prefix){
    
}

std::filesystem::path Archive::extract(std::string module, std::filesystem::path prefix){

}

std::vector<std::filesystem::path> Archive::list(std::filesystem::path prefix){

}