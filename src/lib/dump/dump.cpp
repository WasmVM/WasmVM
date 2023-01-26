/**
 * Copyright 2023 Luis Hsu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include "dump.hpp"

#include <string>
#include <sstream>
#include <utility>

using namespace WasmVM;

static std::ostream& operator<<(std::ostream& stream, WasmModule& module){
    stream << "(module " << module.id << std::endl;
    for(FuncType& type : module.types){
        stream << type;
    }
    stream << ")" << std::endl;
    return stream;
}

std::string WasmVM::module_dump(WasmModule& module){
    std::stringstream stream;
    stream << module;
    return stream.str();
}