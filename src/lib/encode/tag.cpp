// Copyright 2026 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "encode.hpp"
#include "Util.hpp"

using namespace WasmVM;
using namespace Encode;

Encode::Tag::Tag(const std::vector<index_t>& tags) : Section((byte_t)0x0d){
    if(!tags.empty()){
        Encode::Section::Stream stream(buffer);
        // A tag is not a bare typeidx: each entry is an attribute byte
        // (0x00 = exception, the only one defined) followed by the typeidx,
        // which is what Decode::Tag reads back.
        stream << (u32_t)tags.size();
        for(index_t typeidx : tags){
            stream << (byte_t)0x00 << typeidx;
        }
    }
}
