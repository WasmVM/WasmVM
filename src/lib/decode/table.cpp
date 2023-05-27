// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <decode.hpp>
#include "exception.hpp"

using namespace WasmVM;
using namespace Decode;

template<> Stream& Decode::operator>> <TableType>(Stream& stream, TableType& type){
    return stream >> type.reftype >> type.limits;
}

Stream& Decode::Table::read(Stream& stream){
    auto size = header(stream);
    if(size){
        stream >> tables;
    }
    return stream;
}