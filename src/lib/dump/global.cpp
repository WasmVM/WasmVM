// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "dump.hpp"

using namespace WasmVM;

std::ostream& WasmVM::operator<<(std::ostream& stream, WasmGlobal& global){
    stream << "  (global ";
    if(global.type.mut == WasmVM::GlobalType::Mut::variable){
        stream << "(mut " << global.type.type << ") ";
    }else{
        stream << global.type.type << " ";
    }
    return stream << global.init << ")" << std::endl;
}