// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "encode.hpp"

#include <vector>
#include <utility>

using namespace WasmVM;
using namespace Encode;

template<> Section::Stream& Section::Stream::operator<< <WasmFunc>(WasmFunc func){
    // Create buffer & stream
    std::stringstream buffer;
    Encode::Section::Stream stream(buffer);
    // Gather locals
    std::vector<std::pair<ValueType, u32_t>> local_pairs;
    for(ValueType type : func.locals){
        if(local_pairs.empty() || (local_pairs.back().first != type)){
            local_pairs.emplace_back(type, 1);
        }else{
            local_pairs.back().second += 1;
        }
    }
    // emit locals
    stream << (u32_t) local_pairs.size();
    for(std::pair<ValueType, u32_t> local_pair : local_pairs){
        stream << local_pair.second << local_pair.first;
    }
    // emit exprs
    for(WasmInstr& instr : func.body){
        stream << instr;
    }
    // Update output
    return *this << buffer;
}

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

Encode::Code::Code(const std::vector<WasmFunc>& funcs) : Section((byte_t)0x0a){
    if(!funcs.empty()){
        Encode::Section::Stream stream(buffer);
        stream << funcs;
    }
}
