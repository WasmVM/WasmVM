// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "encode.hpp"
#include "Util.hpp"

using namespace WasmVM;
using namespace Encode;

template<> Section::Stream& Section::Stream::operator<< <TableType>(TableType table){
    return *this << table.elemtype << table.limits;
}

Encode::Table::Table(const std::vector<TableType>& tables) : Section((byte_t)0x04){
    if(!tables.empty()){
        Encode::Section::Stream stream(buffer);
        stream << tables;
    }
}