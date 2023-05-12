// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "numeric.hpp"

using namespace WasmVM;

std::optional<Parse::Numeric_f64> Parse::Numeric_f64::get(TokenIter& begin, TokenHolder& holder){
    std::list<TokenType>::iterator it = begin;
    auto syntax = Parse::OneOf<
        Parse::Instr::Atomic<WasmVM::Instr::F64_eq,"f64.eq">, 
        Parse::Instr::Atomic<WasmVM::Instr::F64_ne,"f64.ne">, 
        Parse::Instr::Atomic<WasmVM::Instr::F64_lt,"f64.lt">, 
        Parse::Instr::Atomic<WasmVM::Instr::F64_gt,"f64.gt">, 
        Parse::Instr::Atomic<WasmVM::Instr::F64_le,"f64.le">, 
        Parse::Instr::Atomic<WasmVM::Instr::F64_ge,"f64.ge">,
        Parse::Instr::Atomic<WasmVM::Instr::F64_abs,"f64.abs">, 
        Parse::Instr::Atomic<WasmVM::Instr::F64_neg,"f64.neg">, 
        Parse::Instr::Atomic<WasmVM::Instr::F64_ceil,"f64.ceil">, 
        Parse::Instr::Atomic<WasmVM::Instr::F64_floor,"f64.floor">, 
        Parse::Instr::Atomic<WasmVM::Instr::F64_trunc,"f64.trunc">, 
        Parse::Instr::Atomic<WasmVM::Instr::F64_nearest,"f64.nearest">, 
        Parse::Instr::Atomic<WasmVM::Instr::F64_sqrt,"f64.sqrt">, 
        Parse::Instr::Atomic<WasmVM::Instr::F64_add,"f64.add">, 
        Parse::Instr::Atomic<WasmVM::Instr::F64_sub,"f64.sub">, 
        Parse::Instr::Atomic<WasmVM::Instr::F64_mul,"f64.mul">, 
        Parse::Instr::Atomic<WasmVM::Instr::F64_div,"f64.div">, 
        Parse::Instr::Atomic<WasmVM::Instr::F64_min,"f64.min">, 
        Parse::Instr::Atomic<WasmVM::Instr::F64_max,"f64.max">, 
        Parse::Instr::Atomic<WasmVM::Instr::F64_copysign,"f64.copysign">, 
        Parse::Instr::Atomic<WasmVM::Instr::F64_convert_s_i32,"f64.convert_s_i32">, 
        Parse::Instr::Atomic<WasmVM::Instr::F64_convert_u_i32,"f64.convert_u_i32">, 
        Parse::Instr::Atomic<WasmVM::Instr::F64_convert_s_i64,"f64.convert_s_i64">, 
        Parse::Instr::Atomic<WasmVM::Instr::F64_convert_u_i64,"f64.convert_u_i64">, 
        Parse::Instr::Atomic<WasmVM::Instr::F64_promote_f32,"f64.promote_f32">, 
        Parse::Instr::Atomic<WasmVM::Instr::F64_reinterpret_i64,"f64.reinterpret_i64">
    >::get(it, holder);

    if(syntax){
        begin = it;
        return std::visit(overloaded {
            [&](auto&& instr){
                return Parse::Numeric_f64(Parse::Instr::NumericInstr(instr));
            }
        }, syntax.value());
    }
    return std::nullopt;
}