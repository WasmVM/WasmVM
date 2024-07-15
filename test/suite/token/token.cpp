// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <harness.hpp>
#include <parse/Lexer.hpp>

#include <variant>
#include <sstream>

using namespace WasmVM;
using namespace Testing;

static Token lexing(std::string src){
    std::stringstream ss(src);
    Lexer lexer("text.wat", ss);
    return lexer.get();
}

Suite token {
    Test("parentheses", {
        std::stringstream stream("()");
        Lexer lexer("", stream);
        Expect(std::holds_alternative<Tokens::ParenL>(lexer.get()));
        Expect(std::holds_alternative<Tokens::ParenR>(lexer.get()));
    })
    Test("identifier", {
        Expect(std::holds_alternative<Tokens::Id>(lexing("$testid")));
        Throw(UnknownToken, 
            lexing("notid")
        )
    })
    Test("string", {
        Expect(std::holds_alternative<Tokens::String>(lexing("\"\"")));
        Expect(std::holds_alternative<Tokens::String>(lexing("\"teststr\"")));
        Expect(std::holds_alternative<Tokens::String>(lexing("\"test\\ns\\tt\\r\"")));
        Expect(std::holds_alternative<Tokens::String>(lexing("\"test\\0a\\ttr\"")));
        Expect(std::holds_alternative<Tokens::String>(lexing("\"\\\"\\\\\\\'\"")));
        Expect(std::holds_alternative<Tokens::String>(lexing("\"\\u{e001}\"")));
        Throw(Exception::Parse, lexing("\"test\ns\ttr\""))
        Throw(Exception::Parse, lexing("\""))
        Throw(Exception::Parse, lexing("\"\02\""))
        Throw(Exception::Parse, lexing("\"qsedq\\\""))
        Throw(Exception::Parse, lexing("\"qsedq"))
    })
    Category("number", {
        Test("integer", {
            Expect(std::holds_alternative<Tokens::Integer>(lexing("1234123")));
            Expect(std::holds_alternative<Tokens::Integer>(lexing("000573")));
            Expect(std::holds_alternative<Tokens::Integer>(lexing("+000573")));
            Expect(std::holds_alternative<Tokens::Integer>(lexing("-1_000_000")));
            Expect(std::holds_alternative<Tokens::Integer>(lexing("2_000")));
            Expect(std::holds_alternative<Tokens::Integer>(lexing("+3_000")));
            Throw(Exception::Parse, lexing("1234__123"))
            Throw(UnknownToken, lexing("_56"))
            Expect(std::holds_alternative<Tokens::Integer>(lexing("0x12bc")));
            Expect(std::holds_alternative<Tokens::Integer>(lexing("0x0abcd")));
            Expect(std::holds_alternative<Tokens::Integer>(lexing("+0xcdef")));
            Expect(std::holds_alternative<Tokens::Integer>(lexing("-0x1_0e0_000")));
            Expect(std::holds_alternative<Tokens::Integer>(lexing("0x2_f00")));
            Expect(std::holds_alternative<Tokens::Integer>(lexing("+0x3_0ba")));
            Throw(UnknownToken, lexing("0x+123"))
            Throw(UnknownToken, lexing("00x24"))
            Throw(UnknownToken, lexing("_0x56ac"))
        })
        Test("floating", {
            Expect(std::holds_alternative<Tokens::Float>(lexing("1234.123")));
            Expect(std::holds_alternative<Tokens::Float>(lexing("0.00573")));
            Expect(std::holds_alternative<Tokens::Float>(lexing("+10.0573")));
            Expect(std::holds_alternative<Tokens::Float>(lexing("-1_00.0_040")));
            Expect(std::holds_alternative<Tokens::Float>(lexing("+3.4e-2")));
            Expect(std::holds_alternative<Tokens::Float>(lexing("+3.4e+3")));
            Expect(std::holds_alternative<Tokens::Float>(lexing("+53.4E3")));
            Throw(UnknownToken, lexing("_5.6"))
            Expect(std::holds_alternative<Tokens::Float>(lexing("0x12b.c")));
            Expect(std::holds_alternative<Tokens::Float>(lexing("0x0.abcd")));
            Expect(std::holds_alternative<Tokens::Float>(lexing("+0xcde.f")));
            Expect(std::holds_alternative<Tokens::Float>(lexing("-0x1_2.5p0_003")));
            Expect(std::holds_alternative<Tokens::Float>(lexing("0x2_f0e.4p2")));
            Expect(std::holds_alternative<Tokens::Float>(lexing("+0x3_2P6")));
            Throw(UnknownToken, lexing("_0x5.6ac"))
            Expect(std::holds_alternative<Tokens::Float>(lexing("inf")));
            Expect(std::holds_alternative<Tokens::Float>(lexing("+inf")));
            Expect(std::holds_alternative<Tokens::Float>(lexing("-inf")));
            Expect(std::holds_alternative<Tokens::Float>(lexing("nan")));
            Expect(std::holds_alternative<Tokens::Float>(lexing("nan:0x23")));
        })
    })
    Test("memarg", {
        Expect(std::holds_alternative<Tokens::MemOffset>(lexing("offset=2")));
        Expect(std::holds_alternative<Tokens::MemAlign>(lexing("align=2")));
        Throw(UnknownToken, lexing("align="))
        Throw(UnknownToken, lexing("=4"))
        Throw(UnknownToken, lexing("offset =2"));
        Throw(UnknownToken, lexing("offset = 2"));
        Throw(UnknownToken, lexing("offset= 2"));
    })
    Category("keywords", {
        Test("unknown", {
            Throw(UnknownToken, lexing("testunknown"));
        })
        Test("keyword", {
            Expect(std::holds_alternative<Tokens::Module>(lexing("module")));
            Expect(std::holds_alternative<Tokens::Type>(lexing("type")));
            Expect(std::holds_alternative<Tokens::Import>(lexing("import")));
            Expect(std::holds_alternative<Tokens::Export>(lexing("export")));
            Expect(std::holds_alternative<Tokens::Param>(lexing("param")));
            Expect(std::holds_alternative<Tokens::Result>(lexing("result")));
            Expect(std::holds_alternative<Tokens::Funcref>(lexing("funcref")));
            Expect(std::holds_alternative<Tokens::Extern>(lexing("extern")));
            Expect(std::holds_alternative<Tokens::Externref>(lexing("externref")));
            Expect(std::holds_alternative<Tokens::Func>(lexing("func")));
            Expect(std::holds_alternative<Tokens::Table>(lexing("table")));
            Expect(std::holds_alternative<Tokens::Memory>(lexing("memory")));
            Expect(std::holds_alternative<Tokens::Global>(lexing("global")));
            Expect(std::holds_alternative<Tokens::Mut>(lexing("mut")));
            Expect(std::holds_alternative<Tokens::Local>(lexing("local")));
            Expect(std::holds_alternative<Tokens::Elem>(lexing("elem")));
            Expect(std::holds_alternative<Tokens::Item>(lexing("item")));
            Expect(std::holds_alternative<Tokens::Declare>(lexing("declare")));
            Expect(std::holds_alternative<Tokens::Then>(lexing("then")));
        })
        Test("num_type", {
            Expect(std::holds_alternative<Tokens::NumType>(lexing("i32")));
            Expect(std::holds_alternative<Tokens::NumType>(lexing("i64")));
            Expect(std::holds_alternative<Tokens::NumType>(lexing("f32")));
            Expect(std::holds_alternative<Tokens::NumType>(lexing("f64")));
        })
        Test("control_instruction", {
            Expect(std::holds_alternative<Tokens::Unreachable>(lexing("unreachable")));
            Expect(std::holds_alternative<Tokens::Nop>(lexing("nop")));
            Expect(std::holds_alternative<Tokens::Block>(lexing("block")));
            Expect(std::holds_alternative<Tokens::End>(lexing("end")));
            Expect(std::holds_alternative<Tokens::Loop>(lexing("loop")));
            Expect(std::holds_alternative<Tokens::If>(lexing("if")));
            Expect(std::holds_alternative<Tokens::Else>(lexing("else")));
            Expect(std::holds_alternative<Tokens::Br>(lexing("br")));
            Expect(std::holds_alternative<Tokens::Br_if>(lexing("br_if")));
            Expect(std::holds_alternative<Tokens::Br_table>(lexing("br_table")));
            Expect(std::holds_alternative<Tokens::Return>(lexing("return")));
            Expect(std::holds_alternative<Tokens::Call>(lexing("call")));
            Expect(std::holds_alternative<Tokens::Call_indirect>(lexing("call_indirect")));
        })
        Test("reference_instruction", {
            Expect(std::holds_alternative<Tokens::Ref_null>(lexing("ref.null")));
            Expect(std::holds_alternative<Tokens::Ref_is_null>(lexing("ref.is_null")));
            Expect(std::holds_alternative<Tokens::Ref_func>(lexing("ref.func")));
        })
        Test("parametric_instruction", {
            Expect(std::holds_alternative<Tokens::Drop>(lexing("drop")));
            Expect(std::holds_alternative<Tokens::Select>(lexing("select")));
        })
        Test("variable_instruction", {
            Expect(std::holds_alternative<Tokens::Local_get>(lexing("local.get")));
            Expect(std::holds_alternative<Tokens::Local_set>(lexing("local.set")));
            Expect(std::holds_alternative<Tokens::Local_tee>(lexing("local.tee")));
            Expect(std::holds_alternative<Tokens::Global_get>(lexing("global.get")));
            Expect(std::holds_alternative<Tokens::Global_set>(lexing("global.set")));
        })
        Test("table_instruction", {
            Expect(std::holds_alternative<Tokens::Table_get>(lexing("table.get")));
            Expect(std::holds_alternative<Tokens::Table_set>(lexing("table.set")));
            Expect(std::holds_alternative<Tokens::Table_size>(lexing("table.size")));
            Expect(std::holds_alternative<Tokens::Table_grow>(lexing("table.grow")));
            Expect(std::holds_alternative<Tokens::Table_fill>(lexing("table.fill")));
            Expect(std::holds_alternative<Tokens::Table_copy>(lexing("table.copy")));
            Expect(std::holds_alternative<Tokens::Table_init>(lexing("table.init")));
            Expect(std::holds_alternative<Tokens::Elem_drop>(lexing("elem.drop")));
        })
        Test("memory_instruction", {
            Expect(std::holds_alternative<Tokens::Memory_size>(lexing("memory.size")));
            Expect(std::holds_alternative<Tokens::Memory_grow>(lexing("memory.grow")));
            Expect(std::holds_alternative<Tokens::Memory_fill>(lexing("memory.fill")));
            Expect(std::holds_alternative<Tokens::Memory_init>(lexing("memory.init")));
            Expect(std::holds_alternative<Tokens::Memory_copy>(lexing("memory.copy")));
            Expect(std::holds_alternative<Tokens::Data_drop>(lexing("data.drop")));
            Expect(std::holds_alternative<Tokens::I32_load>(lexing("i32.load")));
            Expect(std::holds_alternative<Tokens::I64_load>(lexing("i64.load")));
            Expect(std::holds_alternative<Tokens::F32_load>(lexing("f32.load")));
            Expect(std::holds_alternative<Tokens::F64_load>(lexing("f64.load")));
            Expect(std::holds_alternative<Tokens::I32_load8_s>(lexing("i32.load8_s")));
            Expect(std::holds_alternative<Tokens::I32_load8_u>(lexing("i32.load8_u")));
            Expect(std::holds_alternative<Tokens::I32_load16_s>(lexing("i32.load16_s")));
            Expect(std::holds_alternative<Tokens::I32_load16_u>(lexing("i32.load16_u")));
            Expect(std::holds_alternative<Tokens::I64_load8_s>(lexing("i64.load8_s")));
            Expect(std::holds_alternative<Tokens::I64_load8_u>(lexing("i64.load8_u")));
            Expect(std::holds_alternative<Tokens::I64_load16_s>(lexing("i64.load16_s")));
            Expect(std::holds_alternative<Tokens::I64_load16_u>(lexing("i64.load16_u")));
            Expect(std::holds_alternative<Tokens::I64_load32_s>(lexing("i64.load32_s")));
            Expect(std::holds_alternative<Tokens::I64_load32_u>(lexing("i64.load32_u")));
            Expect(std::holds_alternative<Tokens::I32_store>(lexing("i32.store")));
            Expect(std::holds_alternative<Tokens::I64_store>(lexing("i64.store")));
            Expect(std::holds_alternative<Tokens::F32_store>(lexing("f32.store")));
            Expect(std::holds_alternative<Tokens::F64_store>(lexing("f64.store")));
            Expect(std::holds_alternative<Tokens::I32_store8>(lexing("i32.store8")));
            Expect(std::holds_alternative<Tokens::I32_store16>(lexing("i32.store16")));
            Expect(std::holds_alternative<Tokens::I64_store8>(lexing("i64.store8")));
            Expect(std::holds_alternative<Tokens::I64_store16>(lexing("i64.store16")));
            Expect(std::holds_alternative<Tokens::I64_store32>(lexing("i64.store32")));
        })
        Test("numeric_instruction", {
            Expect(std::holds_alternative<Tokens::I32_const>(lexing("i32.const")));
            Expect(std::holds_alternative<Tokens::I64_const>(lexing("i64.const")));
            Expect(std::holds_alternative<Tokens::F32_const>(lexing("f32.const")));
            Expect(std::holds_alternative<Tokens::F64_const>(lexing("f64.const")));
            Expect(std::holds_alternative<Tokens::I32_eqz>(lexing("i32.eqz")));
            Expect(std::holds_alternative<Tokens::I32_eq>(lexing("i32.eq")));
            Expect(std::holds_alternative<Tokens::I32_ne>(lexing("i32.ne")));
            Expect(std::holds_alternative<Tokens::I32_lt_s>(lexing("i32.lt_s")));
            Expect(std::holds_alternative<Tokens::I32_lt_u>(lexing("i32.lt_u")));
            Expect(std::holds_alternative<Tokens::I32_gt_s>(lexing("i32.gt_s")));
            Expect(std::holds_alternative<Tokens::I32_gt_u>(lexing("i32.gt_u")));
            Expect(std::holds_alternative<Tokens::I32_le_s>(lexing("i32.le_s")));
            Expect(std::holds_alternative<Tokens::I32_le_u>(lexing("i32.le_u")));
            Expect(std::holds_alternative<Tokens::I32_ge_s>(lexing("i32.ge_s")));
            Expect(std::holds_alternative<Tokens::I32_ge_u>(lexing("i32.ge_u")));
            Expect(std::holds_alternative<Tokens::I64_eqz>(lexing("i64.eqz")));
            Expect(std::holds_alternative<Tokens::I64_eq>(lexing("i64.eq")));
            Expect(std::holds_alternative<Tokens::I64_ne>(lexing("i64.ne")));
            Expect(std::holds_alternative<Tokens::I64_lt_s>(lexing("i64.lt_s")));
            Expect(std::holds_alternative<Tokens::I64_lt_u>(lexing("i64.lt_u")));
            Expect(std::holds_alternative<Tokens::I64_gt_s>(lexing("i64.gt_s")));
            Expect(std::holds_alternative<Tokens::I64_gt_u>(lexing("i64.gt_u")));
            Expect(std::holds_alternative<Tokens::I64_le_s>(lexing("i64.le_s")));
            Expect(std::holds_alternative<Tokens::I64_le_u>(lexing("i64.le_u")));
            Expect(std::holds_alternative<Tokens::I64_ge_s>(lexing("i64.ge_s")));
            Expect(std::holds_alternative<Tokens::I64_ge_u>(lexing("i64.ge_u")));
            Expect(std::holds_alternative<Tokens::F32_eq>(lexing("f32.eq")));
            Expect(std::holds_alternative<Tokens::F32_ne>(lexing("f32.ne")));
            Expect(std::holds_alternative<Tokens::F32_lt>(lexing("f32.lt")));
            Expect(std::holds_alternative<Tokens::F32_gt>(lexing("f32.gt")));
            Expect(std::holds_alternative<Tokens::F32_le>(lexing("f32.le")));
            Expect(std::holds_alternative<Tokens::F32_ge>(lexing("f32.ge")));
            Expect(std::holds_alternative<Tokens::F64_eq>(lexing("f64.eq")));
            Expect(std::holds_alternative<Tokens::F64_ne>(lexing("f64.ne")));
            Expect(std::holds_alternative<Tokens::F64_lt>(lexing("f64.lt")));
            Expect(std::holds_alternative<Tokens::F64_gt>(lexing("f64.gt")));
            Expect(std::holds_alternative<Tokens::F64_le>(lexing("f64.le")));
            Expect(std::holds_alternative<Tokens::F64_ge>(lexing("f64.ge")));
            Expect(std::holds_alternative<Tokens::I32_clz>(lexing("i32.clz")));
            Expect(std::holds_alternative<Tokens::I32_ctz>(lexing("i32.ctz")));
            Expect(std::holds_alternative<Tokens::I32_popcnt>(lexing("i32.popcnt")));
            Expect(std::holds_alternative<Tokens::I32_add>(lexing("i32.add")));
            Expect(std::holds_alternative<Tokens::I32_sub>(lexing("i32.sub")));
            Expect(std::holds_alternative<Tokens::I32_mul>(lexing("i32.mul")));
            Expect(std::holds_alternative<Tokens::I32_div_s>(lexing("i32.div_s")));
            Expect(std::holds_alternative<Tokens::I32_div_u>(lexing("i32.div_u")));
            Expect(std::holds_alternative<Tokens::I32_rem_s>(lexing("i32.rem_s")));
            Expect(std::holds_alternative<Tokens::I32_rem_u>(lexing("i32.rem_u")));
            Expect(std::holds_alternative<Tokens::I32_and>(lexing("i32.and")));
            Expect(std::holds_alternative<Tokens::I32_or>(lexing("i32.or")));
            Expect(std::holds_alternative<Tokens::I32_xor>(lexing("i32.xor")));
            Expect(std::holds_alternative<Tokens::I32_shl>(lexing("i32.shl")));
            Expect(std::holds_alternative<Tokens::I32_shr_s>(lexing("i32.shr_s")));
            Expect(std::holds_alternative<Tokens::I32_shr_u>(lexing("i32.shr_u")));
            Expect(std::holds_alternative<Tokens::I32_rotl>(lexing("i32.rotl")));
            Expect(std::holds_alternative<Tokens::I32_rotr>(lexing("i32.rotr")));
            Expect(std::holds_alternative<Tokens::I64_clz>(lexing("i64.clz")));
            Expect(std::holds_alternative<Tokens::I64_ctz>(lexing("i64.ctz")));
            Expect(std::holds_alternative<Tokens::I64_popcnt>(lexing("i64.popcnt")));
            Expect(std::holds_alternative<Tokens::I64_add>(lexing("i64.add")));
            Expect(std::holds_alternative<Tokens::I64_sub>(lexing("i64.sub")));
            Expect(std::holds_alternative<Tokens::I64_mul>(lexing("i64.mul")));
            Expect(std::holds_alternative<Tokens::I64_div_s>(lexing("i64.div_s")));
            Expect(std::holds_alternative<Tokens::I64_div_u>(lexing("i64.div_u")));
            Expect(std::holds_alternative<Tokens::I64_rem_s>(lexing("i64.rem_s")));
            Expect(std::holds_alternative<Tokens::I64_rem_u>(lexing("i64.rem_u")));
            Expect(std::holds_alternative<Tokens::I64_and>(lexing("i64.and")));
            Expect(std::holds_alternative<Tokens::I64_or>(lexing("i64.or")));
            Expect(std::holds_alternative<Tokens::I64_xor>(lexing("i64.xor")));
            Expect(std::holds_alternative<Tokens::I64_shl>(lexing("i64.shl")));
            Expect(std::holds_alternative<Tokens::I64_shr_s>(lexing("i64.shr_s")));
            Expect(std::holds_alternative<Tokens::I64_shr_u>(lexing("i64.shr_u")));
            Expect(std::holds_alternative<Tokens::I64_rotl>(lexing("i64.rotl")));
            Expect(std::holds_alternative<Tokens::I64_rotr>(lexing("i64.rotr")));
            Expect(std::holds_alternative<Tokens::F32_abs>(lexing("f32.abs")));
            Expect(std::holds_alternative<Tokens::F32_neg>(lexing("f32.neg")));
            Expect(std::holds_alternative<Tokens::F32_ceil>(lexing("f32.ceil")));
            Expect(std::holds_alternative<Tokens::F32_floor>(lexing("f32.floor")));
            Expect(std::holds_alternative<Tokens::F32_trunc>(lexing("f32.trunc")));
            Expect(std::holds_alternative<Tokens::F32_nearest>(lexing("f32.nearest")));
            Expect(std::holds_alternative<Tokens::F32_sqrt>(lexing("f32.sqrt")));
            Expect(std::holds_alternative<Tokens::F32_add>(lexing("f32.add")));
            Expect(std::holds_alternative<Tokens::F32_sub>(lexing("f32.sub")));
            Expect(std::holds_alternative<Tokens::F32_mul>(lexing("f32.mul")));
            Expect(std::holds_alternative<Tokens::F32_div>(lexing("f32.div")));
            Expect(std::holds_alternative<Tokens::F32_min>(lexing("f32.min")));
            Expect(std::holds_alternative<Tokens::F32_max>(lexing("f32.max")));
            Expect(std::holds_alternative<Tokens::F32_copysign>(lexing("f32.copysign")));
            Expect(std::holds_alternative<Tokens::F64_abs>(lexing("f64.abs")));
            Expect(std::holds_alternative<Tokens::F64_neg>(lexing("f64.neg")));
            Expect(std::holds_alternative<Tokens::F64_ceil>(lexing("f64.ceil")));
            Expect(std::holds_alternative<Tokens::F64_floor>(lexing("f64.floor")));
            Expect(std::holds_alternative<Tokens::F64_trunc>(lexing("f64.trunc")));
            Expect(std::holds_alternative<Tokens::F64_nearest>(lexing("f64.nearest")));
            Expect(std::holds_alternative<Tokens::F64_sqrt>(lexing("f64.sqrt")));
            Expect(std::holds_alternative<Tokens::F64_add>(lexing("f64.add")));
            Expect(std::holds_alternative<Tokens::F64_sub>(lexing("f64.sub")));
            Expect(std::holds_alternative<Tokens::F64_mul>(lexing("f64.mul")));
            Expect(std::holds_alternative<Tokens::F64_div>(lexing("f64.div")));
            Expect(std::holds_alternative<Tokens::F64_min>(lexing("f64.min")));
            Expect(std::holds_alternative<Tokens::F64_max>(lexing("f64.max")));
            Expect(std::holds_alternative<Tokens::F64_copysign>(lexing("f64.copysign")));
            Expect(std::holds_alternative<Tokens::I32_wrap_i64>(lexing("i32.wrap_i64")));
            Expect(std::holds_alternative<Tokens::I32_trunc_s_f32>(lexing("i32.trunc_s_f32")));
            Expect(std::holds_alternative<Tokens::I32_trunc_u_f32>(lexing("i32.trunc_u_f32")));
            Expect(std::holds_alternative<Tokens::I32_trunc_s_f64>(lexing("i32.trunc_s_f64")));
            Expect(std::holds_alternative<Tokens::I32_trunc_u_f64>(lexing("i32.trunc_u_f64")));
            Expect(std::holds_alternative<Tokens::I64_extend_s_i32>(lexing("i64.extend_s_i32")));
            Expect(std::holds_alternative<Tokens::I64_extend_u_i32>(lexing("i64.extend_u_i32")));
            Expect(std::holds_alternative<Tokens::I64_trunc_s_f32>(lexing("i64.trunc_s_f32")));
            Expect(std::holds_alternative<Tokens::I64_trunc_u_f32>(lexing("i64.trunc_u_f32")));
            Expect(std::holds_alternative<Tokens::I64_trunc_s_f64>(lexing("i64.trunc_s_f64")));
            Expect(std::holds_alternative<Tokens::I64_trunc_u_f64>(lexing("i64.trunc_u_f64")));
            Expect(std::holds_alternative<Tokens::F32_convert_s_i32>(lexing("f32.convert_s_i32")));
            Expect(std::holds_alternative<Tokens::F32_convert_u_i32>(lexing("f32.convert_u_i32")));
            Expect(std::holds_alternative<Tokens::F32_convert_s_i64>(lexing("f32.convert_s_i64")));
            Expect(std::holds_alternative<Tokens::F32_convert_u_i64>(lexing("f32.convert_u_i64")));
            Expect(std::holds_alternative<Tokens::F32_demote_f64>(lexing("f32.demote_f64")));
            Expect(std::holds_alternative<Tokens::F64_convert_s_i32>(lexing("f64.convert_s_i32")));
            Expect(std::holds_alternative<Tokens::F64_convert_u_i32>(lexing("f64.convert_u_i32")));
            Expect(std::holds_alternative<Tokens::F64_convert_s_i64>(lexing("f64.convert_s_i64")));
            Expect(std::holds_alternative<Tokens::F64_convert_u_i64>(lexing("f64.convert_u_i64")));
            Expect(std::holds_alternative<Tokens::F64_promote_f32>(lexing("f64.promote_f32")));
            Expect(std::holds_alternative<Tokens::I32_reinterpret_f32>(lexing("i32.reinterpret_f32")));
            Expect(std::holds_alternative<Tokens::I64_reinterpret_f64>(lexing("i64.reinterpret_f64")));
            Expect(std::holds_alternative<Tokens::F32_reinterpret_i32>(lexing("f32.reinterpret_i32")));
            Expect(std::holds_alternative<Tokens::F64_reinterpret_i64>(lexing("f64.reinterpret_i64")));
            Expect(std::holds_alternative<Tokens::I32_extend8_s>(lexing("i32.extend8_s")));
            Expect(std::holds_alternative<Tokens::I32_extend16_s>(lexing("i32.extend16_s")));
            Expect(std::holds_alternative<Tokens::I64_extend8_s>(lexing("i64.extend8_s")));
            Expect(std::holds_alternative<Tokens::I64_extend16_s>(lexing("i64.extend16_s")));
            Expect(std::holds_alternative<Tokens::I64_extend32_s>(lexing("i64.extend32_s")));
            Expect(std::holds_alternative<Tokens::I32_trunc_sat_f32_s>(lexing("i32.trunc_sat_f32_s")));
            Expect(std::holds_alternative<Tokens::I32_trunc_sat_f32_u>(lexing("i32.trunc_sat_f32_u")));
            Expect(std::holds_alternative<Tokens::I32_trunc_sat_f64_s>(lexing("i32.trunc_sat_f64_s")));
            Expect(std::holds_alternative<Tokens::I32_trunc_sat_f64_u>(lexing("i32.trunc_sat_f64_u")));
            Expect(std::holds_alternative<Tokens::I64_trunc_sat_f32_s>(lexing("i64.trunc_sat_f32_s")));
            Expect(std::holds_alternative<Tokens::I64_trunc_sat_f32_u>(lexing("i64.trunc_sat_f32_u")));
            Expect(std::holds_alternative<Tokens::I64_trunc_sat_f64_s>(lexing("i64.trunc_sat_f64_s")));
            Expect(std::holds_alternative<Tokens::I64_trunc_sat_f64_u>(lexing("i64.trunc_sat_f64_u")));
        })
    })
};