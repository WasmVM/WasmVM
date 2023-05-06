// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "encode.hpp"

using namespace WasmVM;
using namespace Encode;

template<> Section::Stream& Section::Stream::operator<< <WasmData>(WasmData data){
    switch(data.mode.type){
        case WasmData::DataMode::Mode::active : 
            if(data.mode.memidx){
                *this << (byte_t)0x02 << data.mode.memidx.value();
            }else{
                *this << (byte_t)0x00;
            }
            if(data.mode.offset){
                *this << data.mode.offset.value();
            }else{
                *this << Instr::I32_const(0);
            }
        break;
        case WasmData::DataMode::Mode::passive : 
            *this << (byte_t)0x01;
        break;
    }
    return *this << data.init;
}

Encode::DataCount::DataCount(const std::vector<WasmData>& datas) : Section((byte_t)0x0c){
    if(!datas.empty()){
        Encode::Section::Stream stream(buffer);
        stream << (u32_t)datas.size();
    }
}

Encode::Data::Data(const std::vector<WasmData>& datas) : Section((byte_t)0x0b){
    if(!datas.empty()){
        Encode::Section::Stream stream(buffer);
        stream << datas;
    }
}