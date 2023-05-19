// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "validate.hpp"
#include "state.hpp"
#include <exception.hpp>

using namespace WasmVM;

template<> void Validate::Validator::operator()<MemType>(const MemType& mem){
    if(mem.min > ((offset_t)1 << 48)){
        throw Exception::Exception("memory size too large");
    }
    if(mem.max){
        if(mem.max.value() > ((offset_t)1 << 48)){
            throw Exception::Exception("memory size too large");
        }
        if(mem.min > mem.max.value()){
            throw Exception::Exception("min should not greater than max");
        }
    }
}