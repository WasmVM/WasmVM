// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <decode.hpp>
#include "exception.hpp"

using namespace WasmVM;
using namespace Decode;

template<> Stream& Decode::operator>> <WasmImport>(Stream& stream, WasmImport& import){
    stream >> import.module >> import.name;
    switch(stream.get<byte_t>()){
        case byte_t {0x00}:
            return stream >> import.desc.emplace<index_t>();
        case byte_t {0x01}:
            return stream >> import.desc.emplace<TableType>();
        case byte_t {0x02}:
            return stream >> import.desc.emplace<MemType>();
        case byte_t {0x03}:
            return stream >> import.desc.emplace<GlobalType>();
        default:
            throw Exception::invalid_import(stream.location());
    }
}

Stream& Decode::Import::read(Stream& stream){
    auto size = header(stream);
    if(size){
        stream >> imports;
    }
    return stream;
}