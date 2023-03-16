// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "visitor.hpp"

using namespace WasmVM;

std::optional<Parse::Instr::NumericInstr> Parse::Instr::NumericInstr::get(TokenIter& begin, TokenHolder& holder){
    std::list<TokenType>::iterator it = begin;
    auto syntax = Parse::OneOf<
        Token::Keyword<"i32.eqz">, Token::Keyword<"i32.eq">, Token::Keyword<"i32.ne">, Token::Keyword<"i32.lt_s">, Token::Keyword<"i32.lt_u">, Token::Keyword<"i32.gt_s">, Token::Keyword<"i32.gt_u">, Token::Keyword<"i32.le_s">, Token::Keyword<"i32.le_u">, Token::Keyword<"i32.ge_s">, Token::Keyword<"i32.ge_u">,
        Token::Keyword<"i64.eqz">, Token::Keyword<"i64.eq">, Token::Keyword<"i64.ne">, Token::Keyword<"i64.lt_s">, Token::Keyword<"i64.lt_u">, Token::Keyword<"i64.gt_s">, Token::Keyword<"i64.gt_u">, Token::Keyword<"i64.le_s">, Token::Keyword<"i64.le_u">, Token::Keyword<"i64.ge_s">, Token::Keyword<"i64.ge_u">,
        Token::Keyword<"f32.eq">, Token::Keyword<"f32.ne">, Token::Keyword<"f32.lt">, Token::Keyword<"f32.gt">, Token::Keyword<"f32.le">, Token::Keyword<"f32.ge">, Token::Keyword<"f64.eq">, Token::Keyword<"f64.ne">, Token::Keyword<"f64.lt">, Token::Keyword<"f64.gt">, Token::Keyword<"f64.le">, Token::Keyword<"f64.ge">,
        Token::Keyword<"i32.clz">, Token::Keyword<"i32.ctz">, Token::Keyword<"i32.popcnt">, Token::Keyword<"i32.add">, Token::Keyword<"i32.sub">, Token::Keyword<"i32.mul">, Token::Keyword<"i32.div_s">, Token::Keyword<"i32.div_u">, Token::Keyword<"i32.rem_s">, Token::Keyword<"i32.rem_u">, Token::Keyword<"i32.and">, Token::Keyword<"i32.or">, Token::Keyword<"i32.xor">, Token::Keyword<"i32.shl">, Token::Keyword<"i32.shr_s">, Token::Keyword<"i32.shr_u">, Token::Keyword<"i32.rotl">, Token::Keyword<"i32.rotr">,
        Token::Keyword<"i64.clz">, Token::Keyword<"i64.ctz">, Token::Keyword<"i64.popcnt">, Token::Keyword<"i64.add">, Token::Keyword<"i64.sub">, Token::Keyword<"i64.mul">, Token::Keyword<"i64.div_s">, Token::Keyword<"i64.div_u">, Token::Keyword<"i64.rem_s">, Token::Keyword<"i64.rem_u">, Token::Keyword<"i64.and">, Token::Keyword<"i64.or">, Token::Keyword<"i64.xor">, Token::Keyword<"i64.shl">, Token::Keyword<"i64.shr_s">, Token::Keyword<"i64.shr_u">, Token::Keyword<"i64.rotl">, Token::Keyword<"i64.rotr">, 
        Token::Keyword<"f32.abs">, Token::Keyword<"f32.neg">, Token::Keyword<"f32.ceil">, Token::Keyword<"f32.floor">, Token::Keyword<"f32.trunc">, Token::Keyword<"f32.nearest">, Token::Keyword<"f32.sqrt">, Token::Keyword<"f32.add">, Token::Keyword<"f32.sub">, Token::Keyword<"f32.mul">, Token::Keyword<"f32.div">, Token::Keyword<"f32.min">, Token::Keyword<"f32.max">, Token::Keyword<"f32.copysign">, 
        Token::Keyword<"f64.abs">, Token::Keyword<"f64.neg">, Token::Keyword<"f64.ceil">, Token::Keyword<"f64.floor">, Token::Keyword<"f64.trunc">, Token::Keyword<"f64.nearest">, Token::Keyword<"f64.sqrt">, Token::Keyword<"f64.add">, Token::Keyword<"f64.sub">, Token::Keyword<"f64.mul">, Token::Keyword<"f64.div">, Token::Keyword<"f64.min">, Token::Keyword<"f64.max">, Token::Keyword<"f64.copysign">, 
        Token::Keyword<"i32.wrap_i64">, Token::Keyword<"i32.trunc_s_f32">, Token::Keyword<"i32.trunc_u_f32">, Token::Keyword<"i32.trunc_s_f64">, Token::Keyword<"i32.trunc_u_f64">, Token::Keyword<"i64.extend_s_i32">, Token::Keyword<"i64.extend_u_i32">, Token::Keyword<"i64.trunc_s_f32">, Token::Keyword<"i64.trunc_u_f32">, Token::Keyword<"i64.trunc_s_f64">, Token::Keyword<"i64.trunc_u_f64">, 
        Token::Keyword<"f32.convert_s_i32">, Token::Keyword<"f32.convert_u_i32">, Token::Keyword<"f32.convert_s_i64">, Token::Keyword<"f32.convert_u_i64">, Token::Keyword<"f32.demote_f64">, Token::Keyword<"f64.convert_s_i32">, Token::Keyword<"f64.convert_u_i32">, Token::Keyword<"f64.convert_s_i64">, Token::Keyword<"f64.convert_u_i64">, Token::Keyword<"f64.promote_f32">, 
        Token::Keyword<"i32.reinterpret_f32">, Token::Keyword<"i64.reinterpret_f64">, Token::Keyword<"f32.reinterpret_i32">, Token::Keyword<"f64.reinterpret_i64">, Token::Keyword<"i32.extend8_s">, Token::Keyword<"i32.extend16_s">, Token::Keyword<"i64.extend8_s">, Token::Keyword<"i64.extend16_s">, Token::Keyword<"i64.extend32_s">, 
        Token::Keyword<"i32.trunc_sat_f32_s">, Token::Keyword<"i32.trunc_sat_f32_u">, Token::Keyword<"i32.trunc_sat_f64_s">, Token::Keyword<"i32.trunc_sat_f64_u">, Token::Keyword<"i64.trunc_sat_f32_s">, Token::Keyword<"i64.trunc_sat_f32_u">, Token::Keyword<"i64.trunc_sat_f64_s">, Token::Keyword<"i64.trunc_sat_f64_u">
    >::get(it, holder);

    if(syntax){
        begin = it;
        return Parse::Instr::NumericInstr(syntax.value());
    }
    return std::nullopt;
}

void InstrVisitor::Sema::operator()(Parse::Instr::NumericInstr& node){
    std::visit(overloaded {
        [&](Token::Keyword<"i32.eqz">&){
            func.body.emplace_back(WasmVM::Instr::I32_eqz());
        },
        [&](Token::Keyword<"i32.eq">&){
            func.body.emplace_back(WasmVM::Instr::I32_eq());
        },
        [&](Token::Keyword<"i32.ne">&){
            func.body.emplace_back(WasmVM::Instr::I32_ne());
        },
        [&](Token::Keyword<"i32.lt_s">&){
            func.body.emplace_back(WasmVM::Instr::I32_lt_s());
        },
        [&](Token::Keyword<"i32.lt_u">&){
            func.body.emplace_back(WasmVM::Instr::I32_lt_u());
        },
        [&](Token::Keyword<"i32.gt_s">&){
            func.body.emplace_back(WasmVM::Instr::I32_gt_s());
        },
        [&](Token::Keyword<"i32.gt_u">&){
            func.body.emplace_back(WasmVM::Instr::I32_gt_u());
        },
        [&](Token::Keyword<"i32.le_s">&){
            func.body.emplace_back(WasmVM::Instr::I32_le_s());
        },
        [&](Token::Keyword<"i32.le_u">&){
            func.body.emplace_back(WasmVM::Instr::I32_le_u());
        },
        [&](Token::Keyword<"i32.ge_s">&){
            func.body.emplace_back(WasmVM::Instr::I32_ge_s());
        },
        [&](Token::Keyword<"i32.ge_u">&){
            func.body.emplace_back(WasmVM::Instr::I32_ge_u());
        },
        [&](Token::Keyword<"i64.eqz">&){
            func.body.emplace_back(WasmVM::Instr::I64_eqz());
        },
        [&](Token::Keyword<"i64.eq">&){
            func.body.emplace_back(WasmVM::Instr::I64_eq());
        },
        [&](Token::Keyword<"i64.ne">&){
            func.body.emplace_back(WasmVM::Instr::I64_ne());
        },
        [&](Token::Keyword<"i64.lt_s">&){
            func.body.emplace_back(WasmVM::Instr::I64_lt_s());
        },
        [&](Token::Keyword<"i64.lt_u">&){
            func.body.emplace_back(WasmVM::Instr::I64_lt_u());
        },
        [&](Token::Keyword<"i64.gt_s">&){
            func.body.emplace_back(WasmVM::Instr::I64_gt_s());
        },
        [&](Token::Keyword<"i64.gt_u">&){
            func.body.emplace_back(WasmVM::Instr::I64_gt_u());
        },
        [&](Token::Keyword<"i64.le_s">&){
            func.body.emplace_back(WasmVM::Instr::I64_le_s());
        },
        [&](Token::Keyword<"i64.le_u">&){
            func.body.emplace_back(WasmVM::Instr::I64_le_u());
        },
        [&](Token::Keyword<"i64.ge_s">&){
            func.body.emplace_back(WasmVM::Instr::I64_ge_s());
        },
        [&](Token::Keyword<"i64.ge_u">&){
            func.body.emplace_back(WasmVM::Instr::I64_ge_u());
        },
        [&](Token::Keyword<"f32.eq">&){
            func.body.emplace_back(WasmVM::Instr::F32_eq());
        },
        [&](Token::Keyword<"f32.ne">&){
            func.body.emplace_back(WasmVM::Instr::F32_ne());
        },
        [&](Token::Keyword<"f32.lt">&){
            func.body.emplace_back(WasmVM::Instr::F32_lt());
        },
        [&](Token::Keyword<"f32.gt">&){
            func.body.emplace_back(WasmVM::Instr::F32_gt());
        },
        [&](Token::Keyword<"f32.le">&){
            func.body.emplace_back(WasmVM::Instr::F32_le());
        },
        [&](Token::Keyword<"f32.ge">&){
            func.body.emplace_back(WasmVM::Instr::F32_ge());
        },
        [&](Token::Keyword<"f64.eq">&){
            func.body.emplace_back(WasmVM::Instr::F64_eq());
        },
        [&](Token::Keyword<"f64.ne">&){
            func.body.emplace_back(WasmVM::Instr::F64_ne());
        },
        [&](Token::Keyword<"f64.lt">&){
            func.body.emplace_back(WasmVM::Instr::F64_lt());
        },
        [&](Token::Keyword<"f64.gt">&){
            func.body.emplace_back(WasmVM::Instr::F64_gt());
        },
        [&](Token::Keyword<"f64.le">&){
            func.body.emplace_back(WasmVM::Instr::F64_le());
        },
        [&](Token::Keyword<"f64.ge">&){
            func.body.emplace_back(WasmVM::Instr::F64_ge());
        },
        [&](Token::Keyword<"i32.clz">&){
            func.body.emplace_back(WasmVM::Instr::I32_clz());
        },
        [&](Token::Keyword<"i32.ctz">&){
            func.body.emplace_back(WasmVM::Instr::I32_ctz());
        },
        [&](Token::Keyword<"i32.popcnt">&){
            func.body.emplace_back(WasmVM::Instr::I32_popcnt());
        },
        [&](Token::Keyword<"i32.add">&){
            func.body.emplace_back(WasmVM::Instr::I32_add());
        },
        [&](Token::Keyword<"i32.sub">&){
            func.body.emplace_back(WasmVM::Instr::I32_sub());
        },
        [&](Token::Keyword<"i32.mul">&){
            func.body.emplace_back(WasmVM::Instr::I32_mul());
        },
        [&](Token::Keyword<"i32.div_s">&){
            func.body.emplace_back(WasmVM::Instr::I32_div_s());
        },
        [&](Token::Keyword<"i32.div_u">&){
            func.body.emplace_back(WasmVM::Instr::I32_div_u());
        },
        [&](Token::Keyword<"i32.rem_s">&){
            func.body.emplace_back(WasmVM::Instr::I32_rem_s());
        },
        [&](Token::Keyword<"i32.rem_u">&){
            func.body.emplace_back(WasmVM::Instr::I32_rem_u());
        },
        [&](Token::Keyword<"i32.and">&){
            func.body.emplace_back(WasmVM::Instr::I32_and());
        },
        [&](Token::Keyword<"i32.or">&){
            func.body.emplace_back(WasmVM::Instr::I32_or());
        },
        [&](Token::Keyword<"i32.xor">&){
            func.body.emplace_back(WasmVM::Instr::I32_xor());
        },
        [&](Token::Keyword<"i32.shl">&){
            func.body.emplace_back(WasmVM::Instr::I32_shl());
        },
        [&](Token::Keyword<"i32.shr_s">&){
            func.body.emplace_back(WasmVM::Instr::I32_shr_s());
        },
        [&](Token::Keyword<"i32.shr_u">&){
            func.body.emplace_back(WasmVM::Instr::I32_shr_u());
        },
        [&](Token::Keyword<"i32.rotl">&){
            func.body.emplace_back(WasmVM::Instr::I32_rotl());
        },
        [&](Token::Keyword<"i32.rotr">&){
            func.body.emplace_back(WasmVM::Instr::I32_rotr());
        },
        [&](Token::Keyword<"i64.clz">&){
            func.body.emplace_back(WasmVM::Instr::I64_clz());
        },
        [&](Token::Keyword<"i64.ctz">&){
            func.body.emplace_back(WasmVM::Instr::I64_ctz());
        },
        [&](Token::Keyword<"i64.popcnt">&){
            func.body.emplace_back(WasmVM::Instr::I64_popcnt());
        },
        [&](Token::Keyword<"i64.add">&){
            func.body.emplace_back(WasmVM::Instr::I64_add());
        },
        [&](Token::Keyword<"i64.sub">&){
            func.body.emplace_back(WasmVM::Instr::I64_sub());
        },
        [&](Token::Keyword<"i64.mul">&){
            func.body.emplace_back(WasmVM::Instr::I64_mul());
        },
        [&](Token::Keyword<"i64.div_s">&){
            func.body.emplace_back(WasmVM::Instr::I64_div_s());
        },
        [&](Token::Keyword<"i64.div_u">&){
            func.body.emplace_back(WasmVM::Instr::I64_div_u());
        },
        [&](Token::Keyword<"i64.rem_s">&){
            func.body.emplace_back(WasmVM::Instr::I64_rem_s());
        },
        [&](Token::Keyword<"i64.rem_u">&){
            func.body.emplace_back(WasmVM::Instr::I64_rem_u());
        },
        [&](Token::Keyword<"i64.and">&){
            func.body.emplace_back(WasmVM::Instr::I64_and());
        },
        [&](Token::Keyword<"i64.or">&){
            func.body.emplace_back(WasmVM::Instr::I64_or());
        },
        [&](Token::Keyword<"i64.xor">&){
            func.body.emplace_back(WasmVM::Instr::I64_xor());
        },
        [&](Token::Keyword<"i64.shl">&){
            func.body.emplace_back(WasmVM::Instr::I64_shl());
        },
        [&](Token::Keyword<"i64.shr_s">&){
            func.body.emplace_back(WasmVM::Instr::I64_shr_s());
        },
        [&](Token::Keyword<"i64.shr_u">&){
            func.body.emplace_back(WasmVM::Instr::I64_shr_u());
        },
        [&](Token::Keyword<"i64.rotl">&){
            func.body.emplace_back(WasmVM::Instr::I64_rotl());
        },
        [&](Token::Keyword<"i64.rotr">&){
            func.body.emplace_back(WasmVM::Instr::I64_rotr());
        },
        [&](Token::Keyword<"f32.abs">&){
            func.body.emplace_back(WasmVM::Instr::F32_abs());
        },
        [&](Token::Keyword<"f32.neg">&){
            func.body.emplace_back(WasmVM::Instr::F32_neg());
        },
        [&](Token::Keyword<"f32.ceil">&){
            func.body.emplace_back(WasmVM::Instr::F32_ceil());
        },
        [&](Token::Keyword<"f32.floor">&){
            func.body.emplace_back(WasmVM::Instr::F32_floor());
        },
        [&](Token::Keyword<"f32.trunc">&){
            func.body.emplace_back(WasmVM::Instr::F32_trunc());
        },
        [&](Token::Keyword<"f32.nearest">&){
            func.body.emplace_back(WasmVM::Instr::F32_nearest());
        },
        [&](Token::Keyword<"f32.sqrt">&){
            func.body.emplace_back(WasmVM::Instr::F32_sqrt());
        },
        [&](Token::Keyword<"f32.add">&){
            func.body.emplace_back(WasmVM::Instr::F32_add());
        },
        [&](Token::Keyword<"f32.sub">&){
            func.body.emplace_back(WasmVM::Instr::F32_sub());
        },
        [&](Token::Keyword<"f32.mul">&){
            func.body.emplace_back(WasmVM::Instr::F32_mul());
        },
        [&](Token::Keyword<"f32.div">&){
            func.body.emplace_back(WasmVM::Instr::F32_div());
        },
        [&](Token::Keyword<"f32.min">&){
            func.body.emplace_back(WasmVM::Instr::F32_min());
        },
        [&](Token::Keyword<"f32.max">&){
            func.body.emplace_back(WasmVM::Instr::F32_max());
        },
        [&](Token::Keyword<"f32.copysign">&){
            func.body.emplace_back(WasmVM::Instr::F32_copysign());
        },
        [&](Token::Keyword<"f64.abs">&){
            func.body.emplace_back(WasmVM::Instr::F64_abs());
        },
        [&](Token::Keyword<"f64.neg">&){
            func.body.emplace_back(WasmVM::Instr::F64_neg());
        },
        [&](Token::Keyword<"f64.ceil">&){
            func.body.emplace_back(WasmVM::Instr::F64_ceil());
        },
        [&](Token::Keyword<"f64.floor">&){
            func.body.emplace_back(WasmVM::Instr::F64_floor());
        },
        [&](Token::Keyword<"f64.trunc">&){
            func.body.emplace_back(WasmVM::Instr::F64_trunc());
        },
        [&](Token::Keyword<"f64.nearest">&){
            func.body.emplace_back(WasmVM::Instr::F64_nearest());
        },
        [&](Token::Keyword<"f64.sqrt">&){
            func.body.emplace_back(WasmVM::Instr::F64_sqrt());
        },
        [&](Token::Keyword<"f64.add">&){
            func.body.emplace_back(WasmVM::Instr::F64_add());
        },
        [&](Token::Keyword<"f64.sub">&){
            func.body.emplace_back(WasmVM::Instr::F64_sub());
        },
        [&](Token::Keyword<"f64.mul">&){
            func.body.emplace_back(WasmVM::Instr::F64_mul());
        },
        [&](Token::Keyword<"f64.div">&){
            func.body.emplace_back(WasmVM::Instr::F64_div());
        },
        [&](Token::Keyword<"f64.min">&){
            func.body.emplace_back(WasmVM::Instr::F64_min());
        },
        [&](Token::Keyword<"f64.max">&){
            func.body.emplace_back(WasmVM::Instr::F64_max());
        },
        [&](Token::Keyword<"f64.copysign">&){
            func.body.emplace_back(WasmVM::Instr::F64_copysign());
        },
        [&](Token::Keyword<"i32.wrap_i64">&){
            func.body.emplace_back(WasmVM::Instr::I32_wrap_i64());
        },
        [&](Token::Keyword<"i32.trunc_s_f32">&){
            func.body.emplace_back(WasmVM::Instr::I32_trunc_s_f32());
        },
        [&](Token::Keyword<"i32.trunc_u_f32">&){
            func.body.emplace_back(WasmVM::Instr::I32_trunc_u_f32());
        },
        [&](Token::Keyword<"i32.trunc_s_f64">&){
            func.body.emplace_back(WasmVM::Instr::I32_trunc_s_f64());
        },
        [&](Token::Keyword<"i32.trunc_u_f64">&){
            func.body.emplace_back(WasmVM::Instr::I32_trunc_u_f64());
        },
        [&](Token::Keyword<"i64.extend_s_i32">&){
            func.body.emplace_back(WasmVM::Instr::I64_extend_s_i32());
        },
        [&](Token::Keyword<"i64.extend_u_i32">&){
            func.body.emplace_back(WasmVM::Instr::I64_extend_u_i32());
        },
        [&](Token::Keyword<"i64.trunc_s_f32">&){
            func.body.emplace_back(WasmVM::Instr::I64_trunc_s_f32());
        },
        [&](Token::Keyword<"i64.trunc_u_f32">&){
            func.body.emplace_back(WasmVM::Instr::I64_trunc_u_f32());
        },
        [&](Token::Keyword<"i64.trunc_s_f64">&){
            func.body.emplace_back(WasmVM::Instr::I64_trunc_s_f64());
        },
        [&](Token::Keyword<"i64.trunc_u_f64">&){
            func.body.emplace_back(WasmVM::Instr::I64_trunc_u_f64());
        },
        [&](Token::Keyword<"f32.convert_s_i32">&){
            func.body.emplace_back(WasmVM::Instr::F32_convert_s_i32());
        },
        [&](Token::Keyword<"f32.convert_u_i32">&){
            func.body.emplace_back(WasmVM::Instr::F32_convert_u_i32());
        },
        [&](Token::Keyword<"f32.convert_s_i64">&){
            func.body.emplace_back(WasmVM::Instr::F32_convert_s_i64());
        },
        [&](Token::Keyword<"f32.convert_u_i64">&){
            func.body.emplace_back(WasmVM::Instr::F32_convert_u_i64());
        },
        [&](Token::Keyword<"f32.demote_f64">&){
            func.body.emplace_back(WasmVM::Instr::F32_demote_f64());
        },
        [&](Token::Keyword<"f64.convert_s_i32">&){
            func.body.emplace_back(WasmVM::Instr::F64_convert_s_i32());
        },
        [&](Token::Keyword<"f64.convert_u_i32">&){
            func.body.emplace_back(WasmVM::Instr::F64_convert_u_i32());
        },
        [&](Token::Keyword<"f64.convert_s_i64">&){
            func.body.emplace_back(WasmVM::Instr::F64_convert_s_i64());
        },
        [&](Token::Keyword<"f64.convert_u_i64">&){
            func.body.emplace_back(WasmVM::Instr::F64_convert_u_i64());
        },
        [&](Token::Keyword<"f64.promote_f32">&){
            func.body.emplace_back(WasmVM::Instr::F64_promote_f32());
        },
        [&](Token::Keyword<"i32.reinterpret_f32">&){
            func.body.emplace_back(WasmVM::Instr::I32_reinterpret_f32());
        },
        [&](Token::Keyword<"i64.reinterpret_f64">&){
            func.body.emplace_back(WasmVM::Instr::I64_reinterpret_f64());
        },
        [&](Token::Keyword<"f32.reinterpret_i32">&){
            func.body.emplace_back(WasmVM::Instr::F32_reinterpret_i32());
        },
        [&](Token::Keyword<"f64.reinterpret_i64">&){
            func.body.emplace_back(WasmVM::Instr::F64_reinterpret_i64());
        },
        [&](Token::Keyword<"i32.extend8_s">&){
            func.body.emplace_back(WasmVM::Instr::I32_extend8_s());
        },
        [&](Token::Keyword<"i32.extend16_s">&){
            func.body.emplace_back(WasmVM::Instr::I32_extend16_s());
        },
        [&](Token::Keyword<"i64.extend8_s">&){
            func.body.emplace_back(WasmVM::Instr::I64_extend8_s());
        },
        [&](Token::Keyword<"i64.extend16_s">&){
            func.body.emplace_back(WasmVM::Instr::I64_extend16_s());
        },
        [&](Token::Keyword<"i64.extend32_s">&){
            func.body.emplace_back(WasmVM::Instr::I64_extend32_s());
        },
        [&](Token::Keyword<"i32.trunc_sat_f32_s">&){
            func.body.emplace_back(WasmVM::Instr::I32_trunc_sat_f32_s());
        },
        [&](Token::Keyword<"i32.trunc_sat_f32_u">&){
            func.body.emplace_back(WasmVM::Instr::I32_trunc_sat_f32_u());
        },
        [&](Token::Keyword<"i32.trunc_sat_f64_s">&){
            func.body.emplace_back(WasmVM::Instr::I32_trunc_sat_f64_s());
        },
        [&](Token::Keyword<"i32.trunc_sat_f64_u">&){
            func.body.emplace_back(WasmVM::Instr::I32_trunc_sat_f64_u());
        },
        [&](Token::Keyword<"i64.trunc_sat_f32_s">&){
            func.body.emplace_back(WasmVM::Instr::I64_trunc_sat_f32_s());
        },
        [&](Token::Keyword<"i64.trunc_sat_f32_u">&){
            func.body.emplace_back(WasmVM::Instr::I64_trunc_sat_f32_u());
        },
        [&](Token::Keyword<"i64.trunc_sat_f64_s">&){
            func.body.emplace_back(WasmVM::Instr::I64_trunc_sat_f64_s());
        },
        [&](Token::Keyword<"i64.trunc_sat_f64_u">&){
            func.body.emplace_back(WasmVM::Instr::I64_trunc_sat_f64_u());
        }
    }, node.instr);
}