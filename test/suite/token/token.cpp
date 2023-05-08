// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <harness.hpp>
#include <parse/parse.hpp>

#include <variant>
#include <sstream>

using namespace WasmVM;
using namespace Testing;

static std::list<TokenType> tokenize(std::string src){
    std::stringstream stream(src);
    return tokenize(stream);
}

Suite token {
    Test("parentheses", {
        std::list<TokenType> tokens = tokenize("()");
        Expect(std::holds_alternative<Token::ParenL>(tokens.front()));
        Expect(std::holds_alternative<Token::ParenR>(tokens.back()));
    })
    Test("identifier", {
        Expect(std::holds_alternative<Token::Id>(tokenize("$testid").front()));
        Throw(Exception::unknown_token, 
            tokenize("notid")
        )
    })
    Test("string", {
        Expect(std::holds_alternative<Token::String>(tokenize("\"\"").front()));
        Expect(std::holds_alternative<Token::String>(tokenize("\"teststr\"").front()));
        Expect(std::holds_alternative<Token::String>(tokenize("\"test\\ns\\ttr\"").front()));
        Throw(Exception::invalid_character, tokenize("\"test\ns\ttr\""))
        Throw(Exception::string_not_close, tokenize("\""))
        Throw(Exception::string_not_close, tokenize("\"qsedq\""))
        Throw(Exception::string_not_close, tokenize("\"qsedq\"\""))
    })
    Category("number", {
        Test("integer", {
            Expect(std::holds_alternative<Token::Number>(tokenize("1234123").front()));
            Expect(std::holds_alternative<Token::Number>(tokenize("000573").front()));
            Expect(std::holds_alternative<Token::Number>(tokenize("+000573").front()));
            Expect(std::holds_alternative<Token::Number>(tokenize("-1_000_000").front()));
            Expect(std::holds_alternative<Token::Number>(tokenize("2_000").front()));
            Expect(std::holds_alternative<Token::Number>(tokenize("+3_000").front()));
            Throw(Exception::unknown_token, tokenize("a002"))
            Throw(Exception::unknown_token, tokenize("0+002"))
            Throw(Exception::unknown_token, tokenize("_56"))
            Expect(std::holds_alternative<Token::Number>(tokenize("0x12bc").front()));
            Expect(std::holds_alternative<Token::Number>(tokenize("0x0abcd").front()));
            Expect(std::holds_alternative<Token::Number>(tokenize("+0xcdef").front()));
            Expect(std::holds_alternative<Token::Number>(tokenize("-0x1_0e0_000").front()));
            Expect(std::holds_alternative<Token::Number>(tokenize("0x2_f00").front()));
            Expect(std::holds_alternative<Token::Number>(tokenize("+0x3_0ba").front()));
            Throw(Exception::unknown_token, tokenize("0x+123"))
            Throw(Exception::unknown_token, tokenize("00x24"))
            Throw(Exception::unknown_token, tokenize("_0x56ac"))
            Throw(Exception::unknown_token, tokenize("0x56hc"))
        })
        Test("floating", {
            Expect(std::holds_alternative<Token::Number>(tokenize("1234.123").front()));
            Expect(std::holds_alternative<Token::Number>(tokenize("0.00573").front()));
            Expect(std::holds_alternative<Token::Number>(tokenize("+10.0573").front()));
            Expect(std::holds_alternative<Token::Number>(tokenize("-1_00.0_040").front()));
            Expect(std::holds_alternative<Token::Number>(tokenize("2_000").front()));
            Expect(std::holds_alternative<Token::Number>(tokenize("+3_000").front()));
            Expect(std::holds_alternative<Token::Number>(tokenize("+3.4e-2").front()));
            Expect(std::holds_alternative<Token::Number>(tokenize("+3.4e+3").front()));
            Expect(std::holds_alternative<Token::Number>(tokenize("+53.4E3").front()));
            Throw(Exception::unknown_token, tokenize("0..02"))
            Throw(Exception::unknown_token, tokenize("0+e00.2"))
            Throw(Exception::unknown_token, tokenize("_5.6"))
            Expect(std::holds_alternative<Token::Number>(tokenize("0x12b.c").front()));
            Expect(std::holds_alternative<Token::Number>(tokenize("0x0.abcd").front()));
            Expect(std::holds_alternative<Token::Number>(tokenize("+0xcde.f").front()));
            Expect(std::holds_alternative<Token::Number>(tokenize("-0x1_2.5p0_003").front()));
            Expect(std::holds_alternative<Token::Number>(tokenize("0x2_f0e.4p2").front()));
            Expect(std::holds_alternative<Token::Number>(tokenize("+0x3_2P6").front()));
            Throw(Exception::unknown_token, tokenize("0x123p4f"))
            Throw(Exception::unknown_token, tokenize("00x2.4"))
            Throw(Exception::unknown_token, tokenize("_0x5.6ac"))
            Throw(Exception::unknown_token, tokenize("0x56.hc"))
            Expect(std::holds_alternative<Token::Number>(tokenize("inf").front()));
            Expect(std::holds_alternative<Token::Number>(tokenize("+inf").front()));
            Expect(std::holds_alternative<Token::Number>(tokenize("-inf").front()));
            Expect(std::holds_alternative<Token::Number>(tokenize("nan").front()));
            Expect(std::holds_alternative<Token::Number>(tokenize("nan:0x23").front()));
            Throw(Exception::unknown_token, tokenize("nan:0x2g"))
        })
    })
    Test("memarg", {
        Expect(std::holds_alternative<Token::MemArgBase>(tokenize("offset=2").front()));
        Throw(Exception::unknown_token, tokenize("align="))
        Throw(Exception::unknown_token, tokenize("=4"))
        Throw(Exception::unknown_token, tokenize("offset =2"));
        Throw(Exception::unknown_token, tokenize("offset = 2"));
        Throw(Exception::unknown_token, tokenize("offset= 2"));
    })
    Test("keyword", {
        Throw(Exception::unknown_token, tokenize("testunknown"));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("module").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("type").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("import").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("export").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("param").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("result").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("funcref").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("extern").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("externref").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i32").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i64").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("f32").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("f64").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("func").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("table").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("memory").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("global").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("mut").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("local").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("unreachable").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("nop").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("block").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("end").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("loop").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("if").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("then").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("else").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("br").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("br_if").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("br_table").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("return").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("call").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("call_indirect").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("ref.null").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("ref.is_null").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("ref.func").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("drop").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("select").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("local.get").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("local.set").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("local.tee").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("global.get").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("global.set").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("table.get").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("table.set").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("table.size").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("table.grow").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("table.fill").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("table.copy").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("table.init").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("elem.drop").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("memory.size").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("memory.grow").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("memory.fill").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("memory.init").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("memory.copy").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("data.drop").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i32.load").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i64.load").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("f32.load").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("f64.load").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i32.load8_s").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i32.load8_u").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i32.load16_s").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i32.load16_u").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i64.load8_s").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i64.load8_u").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i64.load16_s").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i64.load16_u").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i64.load32_s").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i64.load32_u").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i32.store").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i64.store").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("f32.store").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("f64.store").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i32.store8").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i32.store16").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i64.store8").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i64.store16").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i64.store32").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i32.const").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i64.const").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("f32.const").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("f64.const").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i32.eqz").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i32.eq").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i32.ne").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i32.lt_s").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i32.lt_u").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i32.gt_s").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i32.gt_u").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i32.le_s").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i32.le_u").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i32.ge_s").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i32.ge_u").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i64.eqz").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i64.eq").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i64.ne").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i64.lt_s").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i64.lt_u").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i64.gt_s").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i64.gt_u").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i64.le_s").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i64.le_u").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i64.ge_s").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i64.ge_u").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("f32.eq").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("f32.ne").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("f32.lt").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("f32.gt").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("f32.le").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("f32.ge").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("f64.eq").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("f64.ne").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("f64.lt").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("f64.gt").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("f64.le").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("f64.ge").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i32.clz").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i32.ctz").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i32.popcnt").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i32.add").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i32.sub").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i32.mul").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i32.div_s").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i32.div_u").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i32.rem_s").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i32.rem_u").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i32.and").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i32.or").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i32.xor").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i32.shl").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i32.shr_s").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i32.shr_u").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i32.rotl").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i32.rotr").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i64.clz").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i64.ctz").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i64.popcnt").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i64.add").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i64.sub").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i64.mul").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i64.div_s").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i64.div_u").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i64.rem_s").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i64.rem_u").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i64.and").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i64.or").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i64.xor").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i64.shl").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i64.shr_s").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i64.shr_u").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i64.rotl").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i64.rotr").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("f32.abs").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("f32.neg").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("f32.ceil").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("f32.floor").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("f32.trunc").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("f32.nearest").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("f32.sqrt").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("f32.add").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("f32.sub").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("f32.mul").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("f32.div").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("f32.min").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("f32.max").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("f32.copysign").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("f64.abs").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("f64.neg").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("f64.ceil").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("f64.floor").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("f64.trunc").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("f64.nearest").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("f64.sqrt").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("f64.add").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("f64.sub").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("f64.mul").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("f64.div").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("f64.min").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("f64.max").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("f64.copysign").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i32.wrap_i64").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i32.trunc_s_f32").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i32.trunc_u_f32").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i32.trunc_s_f64").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i32.trunc_u_f64").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i64.extend_s_i32").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i64.extend_u_i32").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i64.trunc_s_f32").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i64.trunc_u_f32").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i64.trunc_s_f64").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i64.trunc_u_f64").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("f32.convert_s_i32").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("f32.convert_u_i32").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("f32.convert_s_i64").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("f32.convert_u_i64").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("f32.demote_f64").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("f64.convert_s_i32").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("f64.convert_u_i32").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("f64.convert_s_i64").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("f64.convert_u_i64").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("f64.promote_f32").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i32.reinterpret_f32").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i64.reinterpret_f64").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("f32.reinterpret_i32").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("f64.reinterpret_i64").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i32.extend8_s").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i32.extend16_s").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i64.extend8_s").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i64.extend16_s").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i64.extend32_s").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i32.trunc_sat_f32_s").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i32.trunc_sat_f32_u").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i32.trunc_sat_f64_s").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i32.trunc_sat_f64_u").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i64.trunc_sat_f32_s").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i64.trunc_sat_f32_u").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i64.trunc_sat_f64_s").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("i64.trunc_sat_f64_u").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("elem").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("item").front()));
        Expect(std::holds_alternative<Token::KeywordBase>(tokenize("declare").front()));
    })
};