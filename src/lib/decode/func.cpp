// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "decode.hpp"
#include "exception.hpp"

using namespace WasmVM;
using namespace Decode;

Stream& Decode::Func::read(Stream& stream){
    auto size = header(stream);
    if(size){
        std::vector<u32_t> typeidxs;
        stream >> typeidxs;
        funcs.resize(typeidxs.size());
        for(size_t i = 0; i < typeidxs.size(); ++i){
            funcs[i].typeidx = typeidxs[i];
        }
    }
    return stream;
}