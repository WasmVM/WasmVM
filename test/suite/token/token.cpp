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
    // Test("memarg", {
    //     Expect(std::holds_alternative<Tokens::MemArgBase>(lexing("offset=2")));
    //     Throw(UnknownToken, lexing("align="))
    //     Throw(UnknownToken, lexing("=4"))
    //     Throw(UnknownToken, lexing("offset =2"));
    //     Throw(UnknownToken, lexing("offset = 2"));
    //     Throw(UnknownToken, lexing("offset= 2"));
    // })
    // Test("keyword", {
    //     Throw(UnknownToken, lexing("testunknown"));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("module")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("type")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("import")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("export")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("param")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("result")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("funcref")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("extern")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("externref")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i32")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i64")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("f32")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("f64")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("func")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("table")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("memory")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("global")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("mut")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("local")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("unreachable")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("nop")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("block")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("end")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("loop")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("if")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("then")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("else")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("br")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("br_if")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("br_table")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("return")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("call")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("call_indirect")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("ref.null")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("ref.is_null")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("ref.func")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("drop")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("select")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("local.get")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("local.set")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("local.tee")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("global.get")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("global.set")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("table.get")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("table.set")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("table.size")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("table.grow")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("table.fill")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("table.copy")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("table.init")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("elem.drop")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("memory.size")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("memory.grow")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("memory.fill")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("memory.init")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("memory.copy")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("data.drop")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i32.load")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i64.load")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("f32.load")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("f64.load")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i32.load8_s")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i32.load8_u")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i32.load16_s")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i32.load16_u")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i64.load8_s")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i64.load8_u")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i64.load16_s")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i64.load16_u")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i64.load32_s")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i64.load32_u")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i32.store")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i64.store")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("f32.store")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("f64.store")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i32.store8")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i32.store16")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i64.store8")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i64.store16")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i64.store32")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i32.const")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i64.const")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("f32.const")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("f64.const")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i32.eqz")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i32.eq")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i32.ne")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i32.lt_s")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i32.lt_u")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i32.gt_s")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i32.gt_u")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i32.le_s")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i32.le_u")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i32.ge_s")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i32.ge_u")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i64.eqz")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i64.eq")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i64.ne")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i64.lt_s")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i64.lt_u")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i64.gt_s")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i64.gt_u")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i64.le_s")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i64.le_u")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i64.ge_s")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i64.ge_u")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("f32.eq")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("f32.ne")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("f32.lt")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("f32.gt")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("f32.le")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("f32.ge")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("f64.eq")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("f64.ne")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("f64.lt")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("f64.gt")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("f64.le")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("f64.ge")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i32.clz")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i32.ctz")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i32.popcnt")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i32.add")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i32.sub")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i32.mul")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i32.div_s")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i32.div_u")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i32.rem_s")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i32.rem_u")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i32.and")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i32.or")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i32.xor")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i32.shl")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i32.shr_s")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i32.shr_u")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i32.rotl")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i32.rotr")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i64.clz")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i64.ctz")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i64.popcnt")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i64.add")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i64.sub")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i64.mul")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i64.div_s")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i64.div_u")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i64.rem_s")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i64.rem_u")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i64.and")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i64.or")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i64.xor")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i64.shl")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i64.shr_s")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i64.shr_u")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i64.rotl")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i64.rotr")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("f32.abs")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("f32.neg")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("f32.ceil")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("f32.floor")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("f32.trunc")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("f32.nearest")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("f32.sqrt")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("f32.add")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("f32.sub")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("f32.mul")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("f32.div")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("f32.min")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("f32.max")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("f32.copysign")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("f64.abs")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("f64.neg")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("f64.ceil")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("f64.floor")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("f64.trunc")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("f64.nearest")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("f64.sqrt")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("f64.add")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("f64.sub")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("f64.mul")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("f64.div")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("f64.min")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("f64.max")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("f64.copysign")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i32.wrap_i64")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i32.trunc_s_f32")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i32.trunc_u_f32")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i32.trunc_s_f64")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i32.trunc_u_f64")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i64.extend_s_i32")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i64.extend_u_i32")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i64.trunc_s_f32")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i64.trunc_u_f32")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i64.trunc_s_f64")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i64.trunc_u_f64")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("f32.convert_s_i32")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("f32.convert_u_i32")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("f32.convert_s_i64")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("f32.convert_u_i64")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("f32.demote_f64")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("f64.convert_s_i32")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("f64.convert_u_i32")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("f64.convert_s_i64")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("f64.convert_u_i64")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("f64.promote_f32")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i32.reinterpret_f32")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i64.reinterpret_f64")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("f32.reinterpret_i32")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("f64.reinterpret_i64")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i32.extend8_s")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i32.extend16_s")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i64.extend8_s")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i64.extend16_s")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i64.extend32_s")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i32.trunc_sat_f32_s")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i32.trunc_sat_f32_u")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i32.trunc_sat_f64_s")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i32.trunc_sat_f64_u")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i64.trunc_sat_f32_s")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i64.trunc_sat_f32_u")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i64.trunc_sat_f64_s")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("i64.trunc_sat_f64_u")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("elem")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("item")));
    //     Expect(std::holds_alternative<Tokens::KeywordBase>(lexing("declare")));
    // })
};