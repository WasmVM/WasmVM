// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "decode.hpp"
#include "exception.hpp"

using namespace WasmVM;
using namespace Decode;

template<> Stream& Decode::operator>> <GlobalType>(Stream& stream, GlobalType& global){
    stream >> global.type;
    switch(stream.get<byte_t>()){
        case byte_t {0x00}:
            global.mut = GlobalType::Mut::constant;
        break;
        case byte_t {0x01}:
            global.mut = GlobalType::Mut::variable;
        break;
        default:
            throw Exception::invalid_globaltype(stream.location());
    }
    return stream;
}