// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "validate.hpp"
#include "state.hpp"
#include <exception.hpp>

using namespace WasmVM;

template<> void Validate::Validator::operator()<TableType>(const TableType& table){
    if(!table.elemtype.is_ref()){
        throw Exception::Exception("table element type should be a reference type");
    }
    // Every slot starts null, and a non-nullable reference has no null to
    // start as. A table of one would need an initialiser expression, which
    // the type carries no room for.
    if(!table.elemtype.nullable){
        throw Exception::Exception("table element type should be nullable");
    }
    if(table.limits.min >= ((offset_t)1 << 32)){
        throw Exception::Exception("table size too large");
    }
    if(table.limits.max){
        if(table.limits.max.value() > ((offset_t)1 << 48)){
            throw Exception::Exception("table size too large");
        }
        if(table.limits.min > table.limits.max.value()){
            throw Exception::Exception("min should not greater than max");
        }
    }
}