// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "validate.hpp"
#include "state.hpp"
#include <exception.hpp>

using namespace WasmVM;

template<> void Validate::Validator::operator()<TableType>(const TableType& table){
    if(table.limits.max && (table.limits.min > table.limits.max.value())){
        throw Exception::Exception("min should not greater than max");
    }
}