// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <decode.hpp>
#include "exception.hpp"

using namespace WasmVM;
using namespace Decode;

Stream& Decode::Code::read(Stream& stream){
    auto size = header(stream);
    if(size){
        u32_t code_count = stream.get<u32_t>();
        if(code_count != funcs.size()){
            throw Exception::count_mismatch(stream.location());
        }
        for(WasmFunc& func : funcs){
            u32_t code_end = stream.get<u32_t>();
            code_end += stream.location();
            // Locals
            for(u32_t local_count = stream.get<u32_t>(); local_count > 0; --local_count){
                func.locals.insert(func.locals.end(), stream.get<u32_t>(), stream.get<ValueType>());
            }
            while(stream.location() < code_end){
                stream >> func.body.emplace_back();
            }
        }
    }
    return stream;
}

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