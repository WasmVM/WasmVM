// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "../visitor.hpp"

using namespace WasmVM;

std::optional<Parse::Instr::NumericInstr> Parse::Instr::NumericInstr::get(TokenIter& begin, TokenHolder& holder){
    std::list<TokenType>::iterator it = begin;
    auto syntax = Parse::OneOf<
        Parse::Instr::Atomic<WasmVM::Instr::I32_eqz,"i32.eqz">, Parse::Instr::Atomic<WasmVM::Instr::I32_eq,"i32.eq">, Parse::Instr::Atomic<WasmVM::Instr::I32_ne,"i32.ne">, Parse::Instr::Atomic<WasmVM::Instr::I32_lt_s,"i32.lt_s">, Parse::Instr::Atomic<WasmVM::Instr::I32_lt_u,"i32.lt_u">, Parse::Instr::Atomic<WasmVM::Instr::I32_gt_s,"i32.gt_s">, Parse::Instr::Atomic<WasmVM::Instr::I32_gt_u,"i32.gt_u">, Parse::Instr::Atomic<WasmVM::Instr::I32_le_s,"i32.le_s">, Parse::Instr::Atomic<WasmVM::Instr::I32_le_u,"i32.le_u">, Parse::Instr::Atomic<WasmVM::Instr::I32_ge_s,"i32.ge_s">, Parse::Instr::Atomic<WasmVM::Instr::I32_ge_u,"i32.ge_u">,
        Parse::Instr::Atomic<WasmVM::Instr::I64_eqz,"i64.eqz">, Parse::Instr::Atomic<WasmVM::Instr::I64_eq,"i64.eq">, Parse::Instr::Atomic<WasmVM::Instr::I64_ne,"i64.ne">, Parse::Instr::Atomic<WasmVM::Instr::I64_lt_s,"i64.lt_s">, Parse::Instr::Atomic<WasmVM::Instr::I64_lt_u,"i64.lt_u">, Parse::Instr::Atomic<WasmVM::Instr::I64_gt_s,"i64.gt_s">, Parse::Instr::Atomic<WasmVM::Instr::I64_gt_u,"i64.gt_u">, Parse::Instr::Atomic<WasmVM::Instr::I64_le_s,"i64.le_s">, Parse::Instr::Atomic<WasmVM::Instr::I64_le_u,"i64.le_u">, Parse::Instr::Atomic<WasmVM::Instr::I64_ge_s,"i64.ge_s">, Parse::Instr::Atomic<WasmVM::Instr::I64_ge_u,"i64.ge_u">,
        Parse::Instr::Atomic<WasmVM::Instr::F32_eq,"f32.eq">, Parse::Instr::Atomic<WasmVM::Instr::F32_ne,"f32.ne">, Parse::Instr::Atomic<WasmVM::Instr::F32_lt,"f32.lt">, Parse::Instr::Atomic<WasmVM::Instr::F32_gt,"f32.gt">, Parse::Instr::Atomic<WasmVM::Instr::F32_le,"f32.le">, Parse::Instr::Atomic<WasmVM::Instr::F32_ge,"f32.ge">, Parse::Instr::Atomic<WasmVM::Instr::F64_eq,"f64.eq">, Parse::Instr::Atomic<WasmVM::Instr::F64_ne,"f64.ne">, Parse::Instr::Atomic<WasmVM::Instr::F64_lt,"f64.lt">, Parse::Instr::Atomic<WasmVM::Instr::F64_gt,"f64.gt">, Parse::Instr::Atomic<WasmVM::Instr::F64_le,"f64.le">, Parse::Instr::Atomic<WasmVM::Instr::F64_ge,"f64.ge">,
        Parse::Instr::Atomic<WasmVM::Instr::I32_clz,"i32.clz">, Parse::Instr::Atomic<WasmVM::Instr::I32_ctz,"i32.ctz">, Parse::Instr::Atomic<WasmVM::Instr::I32_popcnt,"i32.popcnt">, Parse::Instr::Atomic<WasmVM::Instr::I32_add,"i32.add">, Parse::Instr::Atomic<WasmVM::Instr::I32_sub,"i32.sub">, Parse::Instr::Atomic<WasmVM::Instr::I32_mul,"i32.mul">, Parse::Instr::Atomic<WasmVM::Instr::I32_div_s,"i32.div_s">, Parse::Instr::Atomic<WasmVM::Instr::I32_div_u,"i32.div_u">, Parse::Instr::Atomic<WasmVM::Instr::I32_rem_s,"i32.rem_s">, Parse::Instr::Atomic<WasmVM::Instr::I32_rem_u,"i32.rem_u">, Parse::Instr::Atomic<WasmVM::Instr::I32_and,"i32.and">, Parse::Instr::Atomic<WasmVM::Instr::I32_or,"i32.or">, Parse::Instr::Atomic<WasmVM::Instr::I32_xor,"i32.xor">, Parse::Instr::Atomic<WasmVM::Instr::I32_shl,"i32.shl">, Parse::Instr::Atomic<WasmVM::Instr::I32_shr_s,"i32.shr_s">, Parse::Instr::Atomic<WasmVM::Instr::I32_shr_u,"i32.shr_u">, Parse::Instr::Atomic<WasmVM::Instr::I32_rotl,"i32.rotl">, Parse::Instr::Atomic<WasmVM::Instr::I32_rotr,"i32.rotr">,
        Parse::Instr::Atomic<WasmVM::Instr::I64_clz,"i64.clz">, Parse::Instr::Atomic<WasmVM::Instr::I64_ctz,"i64.ctz">, Parse::Instr::Atomic<WasmVM::Instr::I64_popcnt,"i64.popcnt">, Parse::Instr::Atomic<WasmVM::Instr::I64_add,"i64.add">, Parse::Instr::Atomic<WasmVM::Instr::I64_sub,"i64.sub">, Parse::Instr::Atomic<WasmVM::Instr::I64_mul,"i64.mul">, Parse::Instr::Atomic<WasmVM::Instr::I64_div_s,"i64.div_s">, Parse::Instr::Atomic<WasmVM::Instr::I64_div_u,"i64.div_u">, Parse::Instr::Atomic<WasmVM::Instr::I64_rem_s,"i64.rem_s">, Parse::Instr::Atomic<WasmVM::Instr::I64_rem_u,"i64.rem_u">, Parse::Instr::Atomic<WasmVM::Instr::I64_and,"i64.and">, Parse::Instr::Atomic<WasmVM::Instr::I64_or,"i64.or">, Parse::Instr::Atomic<WasmVM::Instr::I64_xor,"i64.xor">, Parse::Instr::Atomic<WasmVM::Instr::I64_shl,"i64.shl">, Parse::Instr::Atomic<WasmVM::Instr::I64_shr_s,"i64.shr_s">, Parse::Instr::Atomic<WasmVM::Instr::I64_shr_u,"i64.shr_u">, Parse::Instr::Atomic<WasmVM::Instr::I64_rotl,"i64.rotl">, Parse::Instr::Atomic<WasmVM::Instr::I64_rotr,"i64.rotr">, 
        Parse::Instr::Atomic<WasmVM::Instr::F32_abs,"f32.abs">, Parse::Instr::Atomic<WasmVM::Instr::F32_neg,"f32.neg">, Parse::Instr::Atomic<WasmVM::Instr::F32_ceil,"f32.ceil">, Parse::Instr::Atomic<WasmVM::Instr::F32_floor,"f32.floor">, Parse::Instr::Atomic<WasmVM::Instr::F32_trunc,"f32.trunc">, Parse::Instr::Atomic<WasmVM::Instr::F32_nearest,"f32.nearest">, Parse::Instr::Atomic<WasmVM::Instr::F32_sqrt,"f32.sqrt">, Parse::Instr::Atomic<WasmVM::Instr::F32_add,"f32.add">, Parse::Instr::Atomic<WasmVM::Instr::F32_sub,"f32.sub">, Parse::Instr::Atomic<WasmVM::Instr::F32_mul,"f32.mul">, Parse::Instr::Atomic<WasmVM::Instr::F32_div,"f32.div">, Parse::Instr::Atomic<WasmVM::Instr::F32_min,"f32.min">, Parse::Instr::Atomic<WasmVM::Instr::F32_max,"f32.max">, Parse::Instr::Atomic<WasmVM::Instr::F32_copysign,"f32.copysign">, 
        Parse::Instr::Atomic<WasmVM::Instr::F64_abs,"f64.abs">, Parse::Instr::Atomic<WasmVM::Instr::F64_neg,"f64.neg">, Parse::Instr::Atomic<WasmVM::Instr::F64_ceil,"f64.ceil">, Parse::Instr::Atomic<WasmVM::Instr::F64_floor,"f64.floor">, Parse::Instr::Atomic<WasmVM::Instr::F64_trunc,"f64.trunc">, Parse::Instr::Atomic<WasmVM::Instr::F64_nearest,"f64.nearest">, Parse::Instr::Atomic<WasmVM::Instr::F64_sqrt,"f64.sqrt">, Parse::Instr::Atomic<WasmVM::Instr::F64_add,"f64.add">, Parse::Instr::Atomic<WasmVM::Instr::F64_sub,"f64.sub">, Parse::Instr::Atomic<WasmVM::Instr::F64_mul,"f64.mul">, Parse::Instr::Atomic<WasmVM::Instr::F64_div,"f64.div">, Parse::Instr::Atomic<WasmVM::Instr::F64_min,"f64.min">, Parse::Instr::Atomic<WasmVM::Instr::F64_max,"f64.max">, Parse::Instr::Atomic<WasmVM::Instr::F64_copysign,"f64.copysign">, 
        Parse::Instr::Atomic<WasmVM::Instr::I32_wrap_i64,"i32.wrap_i64">, Parse::Instr::Atomic<WasmVM::Instr::I32_trunc_s_f32,"i32.trunc_s_f32">, Parse::Instr::Atomic<WasmVM::Instr::I32_trunc_u_f32,"i32.trunc_u_f32">, Parse::Instr::Atomic<WasmVM::Instr::I32_trunc_s_f64,"i32.trunc_s_f64">, Parse::Instr::Atomic<WasmVM::Instr::I32_trunc_u_f64,"i32.trunc_u_f64">, Parse::Instr::Atomic<WasmVM::Instr::I64_extend_s_i32,"i64.extend_s_i32">, Parse::Instr::Atomic<WasmVM::Instr::I64_extend_u_i32,"i64.extend_u_i32">, Parse::Instr::Atomic<WasmVM::Instr::I64_trunc_s_f32,"i64.trunc_s_f32">, Parse::Instr::Atomic<WasmVM::Instr::I64_trunc_u_f32,"i64.trunc_u_f32">, Parse::Instr::Atomic<WasmVM::Instr::I64_trunc_s_f64,"i64.trunc_s_f64">, Parse::Instr::Atomic<WasmVM::Instr::I64_trunc_u_f64,"i64.trunc_u_f64">, 
        Parse::Instr::Atomic<WasmVM::Instr::F32_convert_s_i32,"f32.convert_s_i32">, Parse::Instr::Atomic<WasmVM::Instr::F32_convert_u_i32,"f32.convert_u_i32">, Parse::Instr::Atomic<WasmVM::Instr::F32_convert_s_i64,"f32.convert_s_i64">, Parse::Instr::Atomic<WasmVM::Instr::F32_convert_u_i64,"f32.convert_u_i64">, Parse::Instr::Atomic<WasmVM::Instr::F32_demote_f64,"f32.demote_f64">, Parse::Instr::Atomic<WasmVM::Instr::F64_convert_s_i32,"f64.convert_s_i32">, Parse::Instr::Atomic<WasmVM::Instr::F64_convert_u_i32,"f64.convert_u_i32">, Parse::Instr::Atomic<WasmVM::Instr::F64_convert_s_i64,"f64.convert_s_i64">, Parse::Instr::Atomic<WasmVM::Instr::F64_convert_u_i64,"f64.convert_u_i64">, Parse::Instr::Atomic<WasmVM::Instr::F64_promote_f32,"f64.promote_f32">, 
        Parse::Instr::Atomic<WasmVM::Instr::I32_reinterpret_f32,"i32.reinterpret_f32">, Parse::Instr::Atomic<WasmVM::Instr::I64_reinterpret_f64,"i64.reinterpret_f64">, Parse::Instr::Atomic<WasmVM::Instr::F32_reinterpret_i32,"f32.reinterpret_i32">, Parse::Instr::Atomic<WasmVM::Instr::F64_reinterpret_i64,"f64.reinterpret_i64">, Parse::Instr::Atomic<WasmVM::Instr::I32_extend8_s,"i32.extend8_s">, Parse::Instr::Atomic<WasmVM::Instr::I32_extend16_s,"i32.extend16_s">, Parse::Instr::Atomic<WasmVM::Instr::I64_extend8_s,"i64.extend8_s">, Parse::Instr::Atomic<WasmVM::Instr::I64_extend16_s,"i64.extend16_s">, Parse::Instr::Atomic<WasmVM::Instr::I64_extend32_s,"i64.extend32_s">, 
        Parse::Instr::Atomic<WasmVM::Instr::I32_trunc_sat_f32_s,"i32.trunc_sat_f32_s">, Parse::Instr::Atomic<WasmVM::Instr::I32_trunc_sat_f32_u,"i32.trunc_sat_f32_u">, Parse::Instr::Atomic<WasmVM::Instr::I32_trunc_sat_f64_s,"i32.trunc_sat_f64_s">, Parse::Instr::Atomic<WasmVM::Instr::I32_trunc_sat_f64_u,"i32.trunc_sat_f64_u">, Parse::Instr::Atomic<WasmVM::Instr::I64_trunc_sat_f32_s,"i64.trunc_sat_f32_s">, Parse::Instr::Atomic<WasmVM::Instr::I64_trunc_sat_f32_u,"i64.trunc_sat_f32_u">, Parse::Instr::Atomic<WasmVM::Instr::I64_trunc_sat_f64_s,"i64.trunc_sat_f64_s">, Parse::Instr::Atomic<WasmVM::Instr::I64_trunc_sat_f64_u,"i64.trunc_sat_f64_u">
    >::get(it, holder);

    if(syntax){
        begin = it;
        return std::visit(overloaded {
            [&](auto&& instr){
                return Parse::Instr::NumericInstr(instr);
            }
        }, syntax.value());
    }
    return std::nullopt;
}

template<> void InstrVisitor::Sema::operator()<Parse::Instr::NumericInstr>(Parse::Instr::NumericInstr& node){
    instrs.emplace_back(node.instr);
}