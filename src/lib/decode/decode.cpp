// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "decode.hpp"
#include "exception.hpp"

using namespace WasmVM;
using namespace Decode;

WasmModule WasmVM::module_decode(std::istream& stream){
    // Check magic & version
    {
        u32_t data;
        stream.read((char*) &data, sizeof(u32_t));
        if(data != Decode::magic){
            throw Exception::incorrect_magic();
        }
        stream.read((char*) &data, sizeof(u32_t));
        if(data != Decode::version){
            throw Exception::incorrect_magic();
        }
    }

    WasmModule module;
    // Types
    return module;
}

using namespace Exception;
incorrect_magic::incorrect_magic() : 
    Decode("incorrect magic & version number", 0) {}
