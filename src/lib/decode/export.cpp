// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <decode.hpp>
#include "exception.hpp"

using namespace WasmVM;
using namespace Decode;

template<> Stream& Decode::operator>> <WasmExport>(Stream& stream, WasmExport& export_){
    stream >> export_.name;
    switch(stream.get<byte_t>()){
        case byte_t {0x00} :
            export_.desc = WasmExport::DescType::func;
        break;
        case byte_t {0x01} :
            export_.desc = WasmExport::DescType::table;
        break;
        case byte_t {0x02} :
            export_.desc = WasmExport::DescType::mem;
        break;
        case byte_t {0x03} :
            export_.desc = WasmExport::DescType::global;
        break;
        default:
            throw Exception::invalid_export(stream.location());
    }
    return stream >> export_.index;
}

Stream& Decode::Export::read(Stream& stream){
    auto size = header(stream);
    if(size){
        stream >> exports;
    }
    return stream;
}