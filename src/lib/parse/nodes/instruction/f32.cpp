// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "numeric.hpp"

using namespace WasmVM;

std::optional<Parse::Numeric_f32> Parse::Numeric_f32::get(TokenIter& begin, TokenHolder& holder){
    std::list<TokenType>::iterator it = begin;
    auto syntax = Parse::OneOf<
        Parse::Instr::Atomic<WasmVM::Instr::F32_eq,"f32.eq">, 
        Parse::Instr::Atomic<WasmVM::Instr::F32_ne,"f32.ne">, 
        Parse::Instr::Atomic<WasmVM::Instr::F32_lt,"f32.lt">, 
        Parse::Instr::Atomic<WasmVM::Instr::F32_gt,"f32.gt">, 
        Parse::Instr::Atomic<WasmVM::Instr::F32_le,"f32.le">, 
        Parse::Instr::Atomic<WasmVM::Instr::F32_ge,"f32.ge">,
        Parse::Instr::Atomic<WasmVM::Instr::F32_abs,"f32.abs">, 
        Parse::Instr::Atomic<WasmVM::Instr::F32_neg,"f32.neg">, 
        Parse::Instr::Atomic<WasmVM::Instr::F32_ceil,"f32.ceil">, 
        Parse::Instr::Atomic<WasmVM::Instr::F32_floor,"f32.floor">, 
        Parse::Instr::Atomic<WasmVM::Instr::F32_trunc,"f32.trunc">, 
        Parse::Instr::Atomic<WasmVM::Instr::F32_nearest,"f32.nearest">, 
        Parse::Instr::Atomic<WasmVM::Instr::F32_sqrt,"f32.sqrt">, 
        Parse::Instr::Atomic<WasmVM::Instr::F32_add,"f32.add">, 
        Parse::Instr::Atomic<WasmVM::Instr::F32_sub,"f32.sub">, 
        Parse::Instr::Atomic<WasmVM::Instr::F32_mul,"f32.mul">, 
        Parse::Instr::Atomic<WasmVM::Instr::F32_div,"f32.div">, 
        Parse::Instr::Atomic<WasmVM::Instr::F32_min,"f32.min">, 
        Parse::Instr::Atomic<WasmVM::Instr::F32_max,"f32.max">, 
        Parse::Instr::Atomic<WasmVM::Instr::F32_copysign,"f32.copysign">, 
        Parse::Instr::Atomic<WasmVM::Instr::F32_convert_s_i32,"f32.convert_s_i32">, 
        Parse::Instr::Atomic<WasmVM::Instr::F32_convert_u_i32,"f32.convert_u_i32">, 
        Parse::Instr::Atomic<WasmVM::Instr::F32_convert_s_i64,"f32.convert_s_i64">, 
        Parse::Instr::Atomic<WasmVM::Instr::F32_convert_u_i64,"f32.convert_u_i64">, 
        Parse::Instr::Atomic<WasmVM::Instr::F32_demote_f64,"f32.demote_f64">,
        Parse::Instr::Atomic<WasmVM::Instr::F32_reinterpret_i32,"f32.reinterpret_i32">
    >::get(it, holder);

    if(syntax){
        begin = it;
        return std::visit(overloaded {
            [&](auto&& instr){
                return Parse::Numeric_f32(Parse::Instr::NumericInstr(instr));
            }
        }, syntax.value());
    }
    return std::nullopt;
}