// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "numeric.hpp"

using namespace WasmVM;

std::optional<Parse::Numeric_i64> Parse::Numeric_i64::get(TokenIter& begin, TokenHolder& holder){
    std::list<TokenType>::iterator it = begin;
    auto syntax = Parse::OneOf<
        Parse::Instr::Atomic<WasmVM::Instr::I64_eqz,"i64.eqz">, 
        Parse::Instr::Atomic<WasmVM::Instr::I64_eq,"i64.eq">, 
        Parse::Instr::Atomic<WasmVM::Instr::I64_ne,"i64.ne">, 
        Parse::Instr::Atomic<WasmVM::Instr::I64_lt_s,"i64.lt_s">, 
        Parse::Instr::Atomic<WasmVM::Instr::I64_lt_u,"i64.lt_u">, 
        Parse::Instr::Atomic<WasmVM::Instr::I64_gt_s,"i64.gt_s">, 
        Parse::Instr::Atomic<WasmVM::Instr::I64_gt_u,"i64.gt_u">, 
        Parse::Instr::Atomic<WasmVM::Instr::I64_le_s,"i64.le_s">, 
        Parse::Instr::Atomic<WasmVM::Instr::I64_le_u,"i64.le_u">, 
        Parse::Instr::Atomic<WasmVM::Instr::I64_ge_s,"i64.ge_s">, 
        Parse::Instr::Atomic<WasmVM::Instr::I64_ge_u,"i64.ge_u">,
        Parse::Instr::Atomic<WasmVM::Instr::I64_clz,"i64.clz">, 
        Parse::Instr::Atomic<WasmVM::Instr::I64_ctz,"i64.ctz">, 
        Parse::Instr::Atomic<WasmVM::Instr::I64_popcnt,"i64.popcnt">, 
        Parse::Instr::Atomic<WasmVM::Instr::I64_add,"i64.add">, 
        Parse::Instr::Atomic<WasmVM::Instr::I64_sub,"i64.sub">, 
        Parse::Instr::Atomic<WasmVM::Instr::I64_mul,"i64.mul">, 
        Parse::Instr::Atomic<WasmVM::Instr::I64_div_s,"i64.div_s">, 
        Parse::Instr::Atomic<WasmVM::Instr::I64_div_u,"i64.div_u">, 
        Parse::Instr::Atomic<WasmVM::Instr::I64_rem_s,"i64.rem_s">, 
        Parse::Instr::Atomic<WasmVM::Instr::I64_rem_u,"i64.rem_u">, 
        Parse::Instr::Atomic<WasmVM::Instr::I64_and,"i64.and">, 
        Parse::Instr::Atomic<WasmVM::Instr::I64_or,"i64.or">, 
        Parse::Instr::Atomic<WasmVM::Instr::I64_xor,"i64.xor">, 
        Parse::Instr::Atomic<WasmVM::Instr::I64_shl,"i64.shl">, 
        Parse::Instr::Atomic<WasmVM::Instr::I64_shr_s,"i64.shr_s">, 
        Parse::Instr::Atomic<WasmVM::Instr::I64_shr_u,"i64.shr_u">, 
        Parse::Instr::Atomic<WasmVM::Instr::I64_rotl,"i64.rotl">, 
        Parse::Instr::Atomic<WasmVM::Instr::I64_rotr,"i64.rotr">, 
        Parse::Instr::Atomic<WasmVM::Instr::I64_extend_s_i32,"i64.extend_s_i32">, 
        Parse::Instr::Atomic<WasmVM::Instr::I64_extend_u_i32,"i64.extend_u_i32">, 
        Parse::Instr::Atomic<WasmVM::Instr::I64_trunc_s_f32,"i64.trunc_s_f32">, 
        Parse::Instr::Atomic<WasmVM::Instr::I64_trunc_u_f32,"i64.trunc_u_f32">, 
        Parse::Instr::Atomic<WasmVM::Instr::I64_trunc_s_f64,"i64.trunc_s_f64">, 
        Parse::Instr::Atomic<WasmVM::Instr::I64_trunc_u_f64,"i64.trunc_u_f64">, 
        Parse::Instr::Atomic<WasmVM::Instr::I64_reinterpret_f64,"i64.reinterpret_f64">,
        Parse::Instr::Atomic<WasmVM::Instr::I64_extend8_s,"i64.extend8_s">,
        Parse::Instr::Atomic<WasmVM::Instr::I64_extend16_s,"i64.extend16_s">,
        Parse::Instr::Atomic<WasmVM::Instr::I64_extend32_s,"i64.extend32_s">,
        Parse::Instr::Atomic<WasmVM::Instr::I64_trunc_sat_f32_s,"i64.trunc_sat_f32_s">,
        Parse::Instr::Atomic<WasmVM::Instr::I64_trunc_sat_f32_u,"i64.trunc_sat_f32_u">,
        Parse::Instr::Atomic<WasmVM::Instr::I64_trunc_sat_f64_s,"i64.trunc_sat_f64_s">,
        Parse::Instr::Atomic<WasmVM::Instr::I64_trunc_sat_f64_u,"i64.trunc_sat_f64_u">
    >::get(it, holder);

    if(syntax){
        begin = it;
        return std::visit(overloaded {
            [&](auto&& instr){
                return Parse::Numeric_i64(Parse::Instr::NumericInstr(instr));
            }
        }, syntax.value());
    }
    return std::nullopt;
}