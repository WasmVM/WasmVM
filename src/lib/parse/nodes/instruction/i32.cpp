// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "numeric.hpp"

using namespace WasmVM;

std::optional<Parse::Numeric_i32> Parse::Numeric_i32::get(TokenIter& begin, TokenHolder& holder){
    std::list<TokenType>::iterator it = begin;
    auto syntax = Parse::OneOf<
        Parse::Instr::Atomic<WasmVM::Instr::I32_eqz,"i32.eqz">,
        Parse::Instr::Atomic<WasmVM::Instr::I32_eq,"i32.eq">,
        Parse::Instr::Atomic<WasmVM::Instr::I32_ne,"i32.ne">,
        Parse::Instr::Atomic<WasmVM::Instr::I32_lt_s,"i32.lt_s">,
        Parse::Instr::Atomic<WasmVM::Instr::I32_lt_u,"i32.lt_u">,
        Parse::Instr::Atomic<WasmVM::Instr::I32_gt_s,"i32.gt_s">,
        Parse::Instr::Atomic<WasmVM::Instr::I32_gt_u,"i32.gt_u">,
        Parse::Instr::Atomic<WasmVM::Instr::I32_le_s,"i32.le_s">,
        Parse::Instr::Atomic<WasmVM::Instr::I32_le_u,"i32.le_u">,
        Parse::Instr::Atomic<WasmVM::Instr::I32_ge_s,"i32.ge_s">,
        Parse::Instr::Atomic<WasmVM::Instr::I32_ge_u,"i32.ge_u">,
        Parse::Instr::Atomic<WasmVM::Instr::I32_clz,"i32.clz">,
        Parse::Instr::Atomic<WasmVM::Instr::I32_ctz,"i32.ctz">,
        Parse::Instr::Atomic<WasmVM::Instr::I32_popcnt,"i32.popcnt">,
        Parse::Instr::Atomic<WasmVM::Instr::I32_add,"i32.add">,
        Parse::Instr::Atomic<WasmVM::Instr::I32_sub,"i32.sub">,
        Parse::Instr::Atomic<WasmVM::Instr::I32_mul,"i32.mul">,
        Parse::Instr::Atomic<WasmVM::Instr::I32_div_s,"i32.div_s">,
        Parse::Instr::Atomic<WasmVM::Instr::I32_div_u,"i32.div_u">,
        Parse::Instr::Atomic<WasmVM::Instr::I32_rem_s,"i32.rem_s">,
        Parse::Instr::Atomic<WasmVM::Instr::I32_rem_u,"i32.rem_u">,
        Parse::Instr::Atomic<WasmVM::Instr::I32_and,"i32.and">,
        Parse::Instr::Atomic<WasmVM::Instr::I32_or,"i32.or">,
        Parse::Instr::Atomic<WasmVM::Instr::I32_xor,"i32.xor">,
        Parse::Instr::Atomic<WasmVM::Instr::I32_shl,"i32.shl">,
        Parse::Instr::Atomic<WasmVM::Instr::I32_shr_s,"i32.shr_s">,
        Parse::Instr::Atomic<WasmVM::Instr::I32_shr_u,"i32.shr_u">,
        Parse::Instr::Atomic<WasmVM::Instr::I32_rotl,"i32.rotl">,
        Parse::Instr::Atomic<WasmVM::Instr::I32_rotr,"i32.rotr">,
        Parse::Instr::Atomic<WasmVM::Instr::I32_wrap_i64,"i32.wrap_i64">,
        Parse::Instr::Atomic<WasmVM::Instr::I32_trunc_s_f32,"i32.trunc_s_f32">,
        Parse::Instr::Atomic<WasmVM::Instr::I32_trunc_u_f32,"i32.trunc_u_f32">,
        Parse::Instr::Atomic<WasmVM::Instr::I32_trunc_s_f64,"i32.trunc_s_f64">,
        Parse::Instr::Atomic<WasmVM::Instr::I32_trunc_u_f64,"i32.trunc_u_f64">,
        Parse::Instr::Atomic<WasmVM::Instr::I32_reinterpret_f32,"i32.reinterpret_f32">,
        Parse::Instr::Atomic<WasmVM::Instr::I32_extend8_s,"i32.extend8_s">,
        Parse::Instr::Atomic<WasmVM::Instr::I32_extend16_s,"i32.extend16_s">,
        Parse::Instr::Atomic<WasmVM::Instr::I32_trunc_sat_f32_s,"i32.trunc_sat_f32_s">,
        Parse::Instr::Atomic<WasmVM::Instr::I32_trunc_sat_f32_u,"i32.trunc_sat_f32_u">,
        Parse::Instr::Atomic<WasmVM::Instr::I32_trunc_sat_f64_s,"i32.trunc_sat_f64_s">,
        Parse::Instr::Atomic<WasmVM::Instr::I32_trunc_sat_f64_u,"i32.trunc_sat_f64_u">
    >::get(it, holder);

    if(syntax){
        begin = it;
        return std::visit(overloaded {
            [&](auto&& instr){
                return Parse::Numeric_i32(Parse::Instr::NumericInstr(instr));
            }
        }, syntax.value());
    }
    return std::nullopt;
}