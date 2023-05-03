// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "encode.hpp"
#include "Util.hpp"

using namespace WasmVM;
using namespace Encode;

Encode::Memory::Memory(const std::vector<MemType>& mems) : Section((byte_t)0x05){
    if(!mems.empty()){
        Encode::Section::Stream stream(buffer);
        stream << mems;
    }
}