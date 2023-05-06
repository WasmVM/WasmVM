// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "encode.hpp"

using namespace WasmVM;
using namespace Encode;

template<> Section::Stream& Section::Stream::operator<< <WasmElem>(WasmElem elem){
    switch(elem.mode.type){
        case WasmElem::ElemMode::Mode::active : 
            if(elem.mode.tableidx){
                *this << (byte_t)0x06 << elem.mode.tableidx.value();
                if(elem.mode.offset){
                    *this << elem.mode.offset.value();
                }else{
                    *this << Instr::I32_const(0);
                }
                return *this << elem.type << elem.elemlist;
            }else{
                *this << (byte_t)0x04;
                if(elem.mode.offset){
                    *this << elem.mode.offset.value();
                }else{
                    *this << Instr::I32_const(0);
                }
                return *this << elem.elemlist;
            }
        break;
        case WasmElem::ElemMode::Mode::passive : 
            return *this << (byte_t)0x05 << elem.type << elem.elemlist;
        break;
        case WasmElem::ElemMode::Mode::declarative : 
            return *this << (byte_t)0x07 << elem.type << elem.elemlist;
        break;
    }
    return *this;
}

Encode::Elem::Elem(const std::vector<WasmElem>& elems) : Section((byte_t)0x09){
    if(!elems.empty()){
        Encode::Section::Stream stream(buffer);
        stream << elems;
    }
}