// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "encode.hpp"
#include "Util.hpp"

using namespace WasmVM;
using namespace Encode;

template<> Section::Stream& Section::Stream::operator<< <WasmImport>(WasmImport import){
    *this << import.module << import.name;
    std::visit(overloaded {
        [&](index_t& typeidx){
            *this << (byte_t)0x00 << typeidx;
        },
        [&](TableType& tabletype){
            *this << (byte_t)0x01 << tabletype;
        },
        [&](MemType& memtype){
            *this << (byte_t)0x02 << memtype;
        },
        [&](GlobalType& globaltype){
            *this << (byte_t)0x03 << globaltype;
        }
    }, import.desc);
    return *this;
}

Encode::Import::Import(const std::vector<WasmImport>& imports) : Section((byte_t)0x02){
    if(!imports.empty()){
        Encode::Section::Stream stream(buffer);
        stream << imports;
    }
}