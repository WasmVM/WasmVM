// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <decode.hpp>
#include "exception.hpp"

using namespace WasmVM;
using namespace Decode;

Stream& Decode::Tag::read(Stream& stream){
    auto size = header(stream);
    if(size){
        u32_t count = stream.get<u32_t>();
        for(u32_t i = 0; i < count; ++i){
            // tagtype: attribute (0x00 = exception) + typeidx
            u8_t attr = (u8_t)stream.get<byte_t>();
            (void)attr;  // currently only 0x00 (exception) is valid
            tags.emplace_back(stream.get<index_t>());
        }
    }
    return stream;
}
