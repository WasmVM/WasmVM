// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "dump.hpp"
#include <Util.hpp>

#include <variant>

std::ostream& WasmVM::operator<<(std::ostream& stream, WasmImport& import){
    stream << "  (import " << import.module << " " << import.name;
    std::visit(overloaded {
        [&](index_t typeidx){
            stream << " (func " << (import.id.empty() ? "" : (import.id + " ")) << "(type " << typeidx << " ) )";
        },
        [&](auto){
            // TODO:
        }
    }, import.desc);
    stream << " )" << std::endl;
    return stream;
}