// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "encode.hpp"

#include <vector>

using namespace WasmVM;
using namespace Encode;

Encode::Func::Func(const std::vector<WasmFunc>& funcs) : Section((byte_t)0x03){
    if(!funcs.empty()){
        Encode::Section::Stream stream(buffer);
        std::vector<index_t> typeidxs;
        for(const WasmFunc& func : funcs){
            typeidxs.emplace_back(func.typeidx);
        }
        stream << typeidxs;
    }
}