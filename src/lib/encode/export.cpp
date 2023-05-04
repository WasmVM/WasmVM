// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "encode.hpp"
#include "Util.hpp"

using namespace WasmVM;
using namespace Encode;

template<> Section::Stream& Section::Stream::operator<< <WasmExport>(WasmExport export_){
    return *this << export_.name << (byte_t)export_.desc << export_.index;
}

Encode::Export::Export(const std::vector<WasmExport>& exports) : Section((byte_t)0x07){
    if(!exports.empty()){
        Encode::Section::Stream stream(buffer);
        stream << exports;
    }
}