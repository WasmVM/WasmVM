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
                // Read the run length and value type as separate, ordered
                // statements. The evaluation order of function arguments is
                // unspecified in C++, so passing both stream.get<>() calls
                // directly to insert() reads them in an ABI-dependent order and
                // desyncs the stream on some targets (e.g. x86-64), corrupting
                // every function that declares locals.
                u32_t count = stream.get<u32_t>();
                ValueType type = stream.get<ValueType>();
                func.locals.insert(func.locals.end(), count, type);
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