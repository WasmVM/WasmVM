// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "encode.hpp"
#include "Util.hpp"

using namespace WasmVM;
using namespace Encode;

template<> Section::Stream& Section::Stream::operator<< <GlobalType>(GlobalType globaltype){
    return *this << globaltype.type << (byte_t)((globaltype.mut == GlobalType::Mut::variable) ? 0x01 : 0x00);
}

template<> Section::Stream& Section::Stream::operator<< <WasmGlobal>(WasmGlobal global){
    return *this << global.type << global.init;
}

Encode::Global::Global(const std::vector<WasmGlobal>& globals) : Section((byte_t)0x06){
    if(!globals.empty()){
        Encode::Section::Stream stream(buffer);
        stream << globals;
    }
}