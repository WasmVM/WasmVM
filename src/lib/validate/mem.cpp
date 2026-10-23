// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "validate.hpp"
#include "state.hpp"
#include <exception.hpp>

using namespace WasmVM;

template<> void Validate::Validator::operator()<MemType>(const MemType& mem){
    // The spec validates a memory type's limits within 2^(|addrtype|-16):
    // 2^16 pages for a 32-bit memory, 2^48 for a 64-bit one, each inclusive.
    // The bound used to be 2^48 for both, so a 32-bit memory could declare up
    // to 2^48 pages. This follows the memory's index type, matching the
    // runtime grow bound in invoke/memory.cpp.
    const offset_t limit = mem.is64 ? ((offset_t)1 << 48) : ((offset_t)1 << 16);
    if(mem.min > limit){
        throw Exception::Exception("memory size too large");
    }
    if(mem.max){
        if(mem.max.value() > limit){
            throw Exception::Exception("memory size too large");
        }
        if(mem.min > mem.max.value()){
            throw Exception::Exception("min should not greater than max");
        }
    }
}