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
            Expect(std::get<Tokens::ControlInstr>(lexing("unreachable")).value == Tokens::ControlInstr::Unreachable);
            Expect(std::get<Tokens::ControlInstr>(lexing("nop")).value == Tokens::ControlInstr::Nop);
            Expect(std::holds_alternative<Tokens::Block>(lexing("block")));
            Expect(std::holds_alternative<Tokens::End>(lexing("end")));
            Expect(std::holds_alternative<Tokens::Loop>(lexing("loop")));
            Expect(std::holds_alternative<Tokens::If>(lexing("if")));
            Expect(std::holds_alternative<Tokens::Else>(lexing("else")));
            Expect(std::holds_alternative<Tokens::Br>(lexing("br")));
            Expect(std::holds_alternative<Tokens::Br_if>(lexing("br_if")));
            Expect(std::holds_alternative<Tokens::Br_table>(lexing("br_table")));
            Expect(std::get<Tokens::ControlInstr>(lexing("return")).value == Tokens::ControlInstr::Return);
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
            Expect(std::get<Tokens::VariableInstr>(lexing("local.get")).value == Tokens::VariableInstr::LocalGet);
            Expect(std::get<Tokens::VariableInstr>(lexing("local.set")).value == Tokens::VariableInstr::LocalSet);
            Expect(std::get<Tokens::VariableInstr>(lexing("local.tee")).value == Tokens::VariableInstr::LocalTee);
            Expect(std::get<Tokens::VariableInstr>(lexing("global.get")).value == Tokens::VariableInstr::GlobalGet);
            Expect(std::get<Tokens::VariableInstr>(lexing("global.set")).value == Tokens::VariableInstr::GlobalSet);
        })
        Test("table_instruction", {
            Expect(std::get<Tokens::TableInstr>(lexing("table.get")).value == Tokens::TableInstr::TableGet);
            Expect(std::get<Tokens::TableInstr>(lexing("table.set")).value == Tokens::TableInstr::TableSet);
            Expect(std::get<Tokens::TableInstr>(lexing("table.size")).value == Tokens::TableInstr::TableSize);
            Expect(std::get<Tokens::TableInstr>(lexing("table.grow")).value == Tokens::TableInstr::TableGrow);
            Expect(std::get<Tokens::TableInstr>(lexing("table.fill")).value == Tokens::TableInstr::TableFill);
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
            Expect(std::get<Tokens::MemoryInstr>(lexing("i32.load")).value == Tokens::MemoryInstr::I32Load);
            Expect(std::get<Tokens::MemoryInstr>(lexing("i64.load")).value == Tokens::MemoryInstr::I64Load);
            Expect(std::get<Tokens::MemoryInstr>(lexing("f32.load")).value == Tokens::MemoryInstr::F32Load);
            Expect(std::get<Tokens::MemoryInstr>(lexing("f64.load")).value == Tokens::MemoryInstr::F64Load);
            Expect(std::get<Tokens::MemoryInstr>(lexing("i32.load8_s")).value == Tokens::MemoryInstr::I32Load8_s);
            Expect(std::get<Tokens::MemoryInstr>(lexing("i32.load8_u")).value == Tokens::MemoryInstr::I32Load8_u);
            Expect(std::get<Tokens::MemoryInstr>(lexing("i32.load16_s")).value == Tokens::MemoryInstr::I32Load16_s);
            Expect(std::get<Tokens::MemoryInstr>(lexing("i32.load16_u")).value == Tokens::MemoryInstr::I32Load16_u);
            Expect(std::get<Tokens::MemoryInstr>(lexing("i64.load8_s")).value == Tokens::MemoryInstr::I64Load8_s);
            Expect(std::get<Tokens::MemoryInstr>(lexing("i64.load8_u")).value == Tokens::MemoryInstr::I64Load8_u);
            Expect(std::get<Tokens::MemoryInstr>(lexing("i64.load16_s")).value == Tokens::MemoryInstr::I64Load16_s);
            Expect(std::get<Tokens::MemoryInstr>(lexing("i64.load16_u")).value == Tokens::MemoryInstr::I64Load16_u);
            Expect(std::get<Tokens::MemoryInstr>(lexing("i64.load32_s")).value == Tokens::MemoryInstr::I64Load32_s);
            Expect(std::get<Tokens::MemoryInstr>(lexing("i64.load32_u")).value == Tokens::MemoryInstr::I64Load32_u);
            Expect(std::get<Tokens::MemoryInstr>(lexing("i32.store")).value == Tokens::MemoryInstr::I32Store);
            Expect(std::get<Tokens::MemoryInstr>(lexing("i64.store")).value == Tokens::MemoryInstr::I64Store);
            // Expect(std::get<Tokens::MemoryInstr>(lexing("f32.store")).value == Tokens::MemoryInstr::F32Store);
            // Expect(std::get<Tokens::MemoryInstr>(lexing("f64.store")).value == Tokens::MemoryInstr::F64Store);
            // Expect(std::get<Tokens::MemoryInstr>(lexing("i32.store8")).value == Tokens::MemoryInstr::I32Store8);
            // Expect(std::get<Tokens::MemoryInstr>(lexing("i32.store16")).value == Tokens::MemoryInstr::I32Store16);
            // Expect(std::get<Tokens::MemoryInstr>(lexing("i64.store8")).value == Tokens::MemoryInstr::I64Store8);
            // Expect(std::get<Tokens::MemoryInstr>(lexing("i64.store16")).value == Tokens::MemoryInstr::I64Store16);
            // Expect(std::get<Tokens::MemoryInstr>(lexing("i64.store32")).value == Tokens::MemoryInstr::I64Store32);
        })
        Test("numeric_instruction", {
            Expect(std::holds_alternative<Tokens::I32_const>(lexing("i32.const")));
            Expect(std::holds_alternative<Tokens::I64_const>(lexing("i64.const")));
            Expect(std::holds_alternative<Tokens::F32_const>(lexing("f32.const")));
            Expect(std::holds_alternative<Tokens::F64_const>(lexing("f64.const")));
            Expect(std::get<Tokens::NumericInstr>(lexing("i32.eqz")).value == Tokens::NumericInstr::I32Eqz);
            Expect(std::get<Tokens::NumericInstr>(lexing("i32.eq")).value == Tokens::NumericInstr::I32Eq);
            Expect(std::get<Tokens::NumericInstr>(lexing("i32.ne")).value == Tokens::NumericInstr::I32Ne);
            Expect(std::get<Tokens::NumericInstr>(lexing("i32.lt_s")).value == Tokens::NumericInstr::I32Lt_s);
            Expect(std::get<Tokens::NumericInstr>(lexing("i32.lt_u")).value == Tokens::NumericInstr::I32Lt_u);
            Expect(std::get<Tokens::NumericInstr>(lexing("i32.gt_s")).value == Tokens::NumericInstr::I32Gt_s);
            Expect(std::get<Tokens::NumericInstr>(lexing("i32.gt_u")).value == Tokens::NumericInstr::I32Gt_u);
            Expect(std::get<Tokens::NumericInstr>(lexing("i32.le_s")).value == Tokens::NumericInstr::I32Le_s);
            Expect(std::get<Tokens::NumericInstr>(lexing("i32.le_u")).value == Tokens::NumericInstr::I32Le_u);
            Expect(std::get<Tokens::NumericInstr>(lexing("i32.ge_s")).value == Tokens::NumericInstr::I32Ge_s);
            Expect(std::get<Tokens::NumericInstr>(lexing("i32.ge_u")).value == Tokens::NumericInstr::I32Ge_u);
            Expect(std::get<Tokens::NumericInstr>(lexing("i64.eqz")).value == Tokens::NumericInstr::I64Eqz);
            Expect(std::get<Tokens::NumericInstr>(lexing("i64.eq")).value == Tokens::NumericInstr::I64Eq);
            Expect(std::get<Tokens::NumericInstr>(lexing("i64.ne")).value == Tokens::NumericInstr::I64Ne);
            Expect(std::get<Tokens::NumericInstr>(lexing("i64.lt_s")).value == Tokens::NumericInstr::I64Lt_s);
            Expect(std::get<Tokens::NumericInstr>(lexing("i64.lt_u")).value == Tokens::NumericInstr::I64Lt_u);
            Expect(std::get<Tokens::NumericInstr>(lexing("i64.gt_s")).value == Tokens::NumericInstr::I64Gt_s);
            Expect(std::get<Tokens::NumericInstr>(lexing("i64.gt_u")).value == Tokens::NumericInstr::I64Gt_u);
            Expect(std::get<Tokens::NumericInstr>(lexing("i64.le_s")).value == Tokens::NumericInstr::I64Le_s);
            Expect(std::get<Tokens::NumericInstr>(lexing("i64.le_u")).value == Tokens::NumericInstr::I64Le_u);
            Expect(std::get<Tokens::NumericInstr>(lexing("i64.ge_s")).value == Tokens::NumericInstr::I64Ge_s);
            Expect(std::get<Tokens::NumericInstr>(lexing("i64.ge_u")).value == Tokens::NumericInstr::I64Ge_u);
            Expect(std::get<Tokens::NumericInstr>(lexing("f32.eq")).value == Tokens::NumericInstr::F32Eq);
            Expect(std::get<Tokens::NumericInstr>(lexing("f32.ne")).value == Tokens::NumericInstr::F32Ne);
            Expect(std::get<Tokens::NumericInstr>(lexing("f32.lt")).value == Tokens::NumericInstr::F32Lt);
            Expect(std::get<Tokens::NumericInstr>(lexing("f32.gt")).value == Tokens::NumericInstr::F32Gt);
            Expect(std::get<Tokens::NumericInstr>(lexing("f32.le")).value == Tokens::NumericInstr::F32Le);
            Expect(std::get<Tokens::NumericInstr>(lexing("f32.ge")).value == Tokens::NumericInstr::F32Ge);
            Expect(std::get<Tokens::NumericInstr>(lexing("f64.eq")).value == Tokens::NumericInstr::F64Eq);
            Expect(std::get<Tokens::NumericInstr>(lexing("f64.ne")).value == Tokens::NumericInstr::F64Ne);
            Expect(std::get<Tokens::NumericInstr>(lexing("f64.lt")).value == Tokens::NumericInstr::F64Lt);
            Expect(std::get<Tokens::NumericInstr>(lexing("f64.gt")).value == Tokens::NumericInstr::F64Gt);
            Expect(std::get<Tokens::NumericInstr>(lexing("f64.le")).value == Tokens::NumericInstr::F64Le);
            Expect(std::get<Tokens::NumericInstr>(lexing("f64.ge")).value == Tokens::NumericInstr::F64Ge);
            Expect(std::get<Tokens::NumericInstr>(lexing("i32.clz")).value == Tokens::NumericInstr::I32Clz);
            Expect(std::get<Tokens::NumericInstr>(lexing("i32.ctz")).value == Tokens::NumericInstr::I32Ctz);
            Expect(std::get<Tokens::NumericInstr>(lexing("i32.popcnt")).value == Tokens::NumericInstr::I32Popcnt);
            Expect(std::get<Tokens::NumericInstr>(lexing("i32.add")).value == Tokens::NumericInstr::I32Add);
            Expect(std::get<Tokens::NumericInstr>(lexing("i32.sub")).value == Tokens::NumericInstr::I32Sub);
            Expect(std::get<Tokens::NumericInstr>(lexing("i32.mul")).value == Tokens::NumericInstr::I32Mul);
            Expect(std::get<Tokens::NumericInstr>(lexing("i32.div_s")).value == Tokens::NumericInstr::I32Div_s);
            Expect(std::get<Tokens::NumericInstr>(lexing("i32.div_u")).value == Tokens::NumericInstr::I32Div_u);
            Expect(std::get<Tokens::NumericInstr>(lexing("i32.rem_s")).value == Tokens::NumericInstr::I32Rem_s);
            Expect(std::get<Tokens::NumericInstr>(lexing("i32.rem_u")).value == Tokens::NumericInstr::I32Rem_u);
            Expect(std::get<Tokens::NumericInstr>(lexing("i32.and")).value == Tokens::NumericInstr::I32And);
            Expect(std::get<Tokens::NumericInstr>(lexing("i32.or")).value == Tokens::NumericInstr::I32Or);
            Expect(std::get<Tokens::NumericInstr>(lexing("i32.xor")).value == Tokens::NumericInstr::I32Xor);
            Expect(std::get<Tokens::NumericInstr>(lexing("i32.shl")).value == Tokens::NumericInstr::I32Shl);
            Expect(std::get<Tokens::NumericInstr>(lexing("i32.shr_s")).value == Tokens::NumericInstr::I32Shr_s);
            Expect(std::get<Tokens::NumericInstr>(lexing("i32.shr_u")).value == Tokens::NumericInstr::I32Shr_u);
            Expect(std::get<Tokens::NumericInstr>(lexing("i32.rotl")).value == Tokens::NumericInstr::I32Rotl);
            Expect(std::get<Tokens::NumericInstr>(lexing("i32.rotr")).value == Tokens::NumericInstr::I32Rotr);
            Expect(std::get<Tokens::NumericInstr>(lexing("i64.clz")).value == Tokens::NumericInstr::I64Clz);
            Expect(std::get<Tokens::NumericInstr>(lexing("i64.ctz")).value == Tokens::NumericInstr::I64Ctz);
            Expect(std::get<Tokens::NumericInstr>(lexing("i64.popcnt")).value == Tokens::NumericInstr::I64Popcnt);
            Expect(std::get<Tokens::NumericInstr>(lexing("i64.add")).value == Tokens::NumericInstr::I64Add);
            Expect(std::get<Tokens::NumericInstr>(lexing("i64.sub")).value == Tokens::NumericInstr::I64Sub);
            Expect(std::get<Tokens::NumericInstr>(lexing("i64.mul")).value == Tokens::NumericInstr::I64Mul);
            Expect(std::get<Tokens::NumericInstr>(lexing("i64.div_s")).value == Tokens::NumericInstr::I64Div_s);
            Expect(std::get<Tokens::NumericInstr>(lexing("i64.div_u")).value == Tokens::NumericInstr::I64Div_u);
            Expect(std::get<Tokens::NumericInstr>(lexing("i64.rem_s")).value == Tokens::NumericInstr::I64Rem_s);
            Expect(std::get<Tokens::NumericInstr>(lexing("i64.rem_u")).value == Tokens::NumericInstr::I64Rem_u);
            Expect(std::get<Tokens::NumericInstr>(lexing("i64.and")).value == Tokens::NumericInstr::I64And);
            Expect(std::get<Tokens::NumericInstr>(lexing("i64.or")).value == Tokens::NumericInstr::I64Or);
            Expect(std::get<Tokens::NumericInstr>(lexing("i64.xor")).value == Tokens::NumericInstr::I64Xor);
            Expect(std::get<Tokens::NumericInstr>(lexing("i64.shl")).value == Tokens::NumericInstr::I64Shl);
            Expect(std::get<Tokens::NumericInstr>(lexing("i64.shr_s")).value == Tokens::NumericInstr::I64Shr_s);
            Expect(std::get<Tokens::NumericInstr>(lexing("i64.shr_u")).value == Tokens::NumericInstr::I64Shr_u);
            Expect(std::get<Tokens::NumericInstr>(lexing("i64.rotl")).value == Tokens::NumericInstr::I64Rotl);
            Expect(std::get<Tokens::NumericInstr>(lexing("i64.rotr")).value == Tokens::NumericInstr::I64Rotr);
            Expect(std::get<Tokens::NumericInstr>(lexing("f32.abs")).value == Tokens::NumericInstr::F32Abs);
            Expect(std::get<Tokens::NumericInstr>(lexing("f32.neg")).value == Tokens::NumericInstr::F32Neg);
            Expect(std::get<Tokens::NumericInstr>(lexing("f32.ceil")).value == Tokens::NumericInstr::F32Ceil);
            Expect(std::get<Tokens::NumericInstr>(lexing("f32.floor")).value == Tokens::NumericInstr::F32Floor);
            Expect(std::get<Tokens::NumericInstr>(lexing("f32.trunc")).value == Tokens::NumericInstr::F32Trunc);
            Expect(std::get<Tokens::NumericInstr>(lexing("f32.nearest")).value == Tokens::NumericInstr::F32Nearest);
            Expect(std::get<Tokens::NumericInstr>(lexing("f32.sqrt")).value == Tokens::NumericInstr::F32Sqrt);
            Expect(std::get<Tokens::NumericInstr>(lexing("f32.add")).value == Tokens::NumericInstr::F32Add);
            Expect(std::get<Tokens::NumericInstr>(lexing("f32.sub")).value == Tokens::NumericInstr::F32Sub);
            Expect(std::get<Tokens::NumericInstr>(lexing("f32.mul")).value == Tokens::NumericInstr::F32Mul);
            Expect(std::get<Tokens::NumericInstr>(lexing("f32.div")).value == Tokens::NumericInstr::F32Div);
            Expect(std::get<Tokens::NumericInstr>(lexing("f32.min")).value == Tokens::NumericInstr::F32Min);
            Expect(std::get<Tokens::NumericInstr>(lexing("f32.max")).value == Tokens::NumericInstr::F32Max);
            Expect(std::get<Tokens::NumericInstr>(lexing("f32.copysign")).value == Tokens::NumericInstr::F32Copysign);
            Expect(std::get<Tokens::NumericInstr>(lexing("f64.abs")).value == Tokens::NumericInstr::F64Abs);
            Expect(std::get<Tokens::NumericInstr>(lexing("f64.neg")).value == Tokens::NumericInstr::F64Neg);
            Expect(std::get<Tokens::NumericInstr>(lexing("f64.ceil")).value == Tokens::NumericInstr::F64Ceil);
            Expect(std::get<Tokens::NumericInstr>(lexing("f64.floor")).value == Tokens::NumericInstr::F64Floor);
            Expect(std::get<Tokens::NumericInstr>(lexing("f64.trunc")).value == Tokens::NumericInstr::F64Trunc);
            Expect(std::get<Tokens::NumericInstr>(lexing("f64.nearest")).value == Tokens::NumericInstr::F64Nearest);
            Expect(std::get<Tokens::NumericInstr>(lexing("f64.sqrt")).value == Tokens::NumericInstr::F64Sqrt);
            Expect(std::get<Tokens::NumericInstr>(lexing("f64.add")).value == Tokens::NumericInstr::F64Add);
            Expect(std::get<Tokens::NumericInstr>(lexing("f64.sub")).value == Tokens::NumericInstr::F64Sub);
            Expect(std::get<Tokens::NumericInstr>(lexing("f64.mul")).value == Tokens::NumericInstr::F64Mul);
            Expect(std::get<Tokens::NumericInstr>(lexing("f64.div")).value == Tokens::NumericInstr::F64Div);
            Expect(std::get<Tokens::NumericInstr>(lexing("f64.min")).value == Tokens::NumericInstr::F64Min);
            Expect(std::get<Tokens::NumericInstr>(lexing("f64.max")).value == Tokens::NumericInstr::F64Max);
            Expect(std::get<Tokens::NumericInstr>(lexing("f64.copysign")).value == Tokens::NumericInstr::F64Copysign);
            Expect(std::get<Tokens::NumericInstr>(lexing("i32.wrap_i64")).value == Tokens::NumericInstr::I32Wrap_i64);
            Expect(std::get<Tokens::NumericInstr>(lexing("i32.trunc_s_f32")).value == Tokens::NumericInstr::I32Trunc_s_f32);
            Expect(std::get<Tokens::NumericInstr>(lexing("i32.trunc_u_f32")).value == Tokens::NumericInstr::I32Trunc_u_f32);
            Expect(std::get<Tokens::NumericInstr>(lexing("i32.trunc_s_f64")).value == Tokens::NumericInstr::I32Trunc_s_f64);
            Expect(std::get<Tokens::NumericInstr>(lexing("i32.trunc_u_f64")).value == Tokens::NumericInstr::I32Trunc_u_f64);
            Expect(std::get<Tokens::NumericInstr>(lexing("i64.extend_s_i32")).value == Tokens::NumericInstr::I64Extend_s_i32);
            Expect(std::get<Tokens::NumericInstr>(lexing("i64.extend_u_i32")).value == Tokens::NumericInstr::I64Extend_u_i32);
            Expect(std::get<Tokens::NumericInstr>(lexing("i64.trunc_s_f32")).value == Tokens::NumericInstr::I64Trunc_s_f32);
            Expect(std::get<Tokens::NumericInstr>(lexing("i64.trunc_u_f32")).value == Tokens::NumericInstr::I64Trunc_u_f32);
            Expect(std::get<Tokens::NumericInstr>(lexing("i64.trunc_s_f64")).value == Tokens::NumericInstr::I64Trunc_s_f64);
            Expect(std::get<Tokens::NumericInstr>(lexing("i64.trunc_u_f64")).value == Tokens::NumericInstr::I64Trunc_u_f64);
            Expect(std::get<Tokens::NumericInstr>(lexing("f32.convert_s_i32")).value == Tokens::NumericInstr::F32Convert_s_i32);
            Expect(std::get<Tokens::NumericInstr>(lexing("f32.convert_u_i32")).value == Tokens::NumericInstr::F32Convert_u_i32);
            Expect(std::get<Tokens::NumericInstr>(lexing("f32.convert_s_i64")).value == Tokens::NumericInstr::F32Convert_s_i64);
            Expect(std::get<Tokens::NumericInstr>(lexing("f32.convert_u_i64")).value == Tokens::NumericInstr::F32Convert_u_i64);
            Expect(std::get<Tokens::NumericInstr>(lexing("f32.demote_f64")).value == Tokens::NumericInstr::F32Demote_f64);
            Expect(std::get<Tokens::NumericInstr>(lexing("f64.convert_s_i32")).value == Tokens::NumericInstr::F64Convert_s_i32);
            Expect(std::get<Tokens::NumericInstr>(lexing("f64.convert_u_i32")).value == Tokens::NumericInstr::F64Convert_u_i32);
            Expect(std::get<Tokens::NumericInstr>(lexing("f64.convert_s_i64")).value == Tokens::NumericInstr::F64Convert_s_i64);
            Expect(std::get<Tokens::NumericInstr>(lexing("f64.convert_u_i64")).value == Tokens::NumericInstr::F64Convert_u_i64);
            Expect(std::get<Tokens::NumericInstr>(lexing("f64.promote_f32")).value == Tokens::NumericInstr::F64Promote_f32);
            Expect(std::get<Tokens::NumericInstr>(lexing("i32.reinterpret_f32")).value == Tokens::NumericInstr::I32Reinterpret_f32);
            Expect(std::get<Tokens::NumericInstr>(lexing("i64.reinterpret_f64")).value == Tokens::NumericInstr::I64Reinterpret_f64);
            Expect(std::get<Tokens::NumericInstr>(lexing("f32.reinterpret_i32")).value == Tokens::NumericInstr::F32Reinterpret_i32);
            Expect(std::get<Tokens::NumericInstr>(lexing("f64.reinterpret_i64")).value == Tokens::NumericInstr::F64Reinterpret_i64);
            Expect(std::get<Tokens::NumericInstr>(lexing("i32.extend8_s")).value == Tokens::NumericInstr::I32Extend8_s);
            Expect(std::get<Tokens::NumericInstr>(lexing("i32.extend16_s")).value == Tokens::NumericInstr::I32Extend16_s);
            Expect(std::get<Tokens::NumericInstr>(lexing("i64.extend8_s")).value == Tokens::NumericInstr::I64Extend8_s);
            Expect(std::get<Tokens::NumericInstr>(lexing("i64.extend16_s")).value == Tokens::NumericInstr::I64Extend16_s);
            Expect(std::get<Tokens::NumericInstr>(lexing("i64.extend32_s")).value == Tokens::NumericInstr::I64Extend32_s);
            Expect(std::get<Tokens::NumericInstr>(lexing("i32.trunc_sat_f32_s")).value == Tokens::NumericInstr::I32Trunc_sat_f32_s);
            Expect(std::get<Tokens::NumericInstr>(lexing("i32.trunc_sat_f32_u")).value == Tokens::NumericInstr::I32Trunc_sat_f32_u);
            Expect(std::get<Tokens::NumericInstr>(lexing("i32.trunc_sat_f64_s")).value == Tokens::NumericInstr::I32Trunc_sat_f64_s);
            Expect(std::get<Tokens::NumericInstr>(lexing("i32.trunc_sat_f64_u")).value == Tokens::NumericInstr::I32Trunc_sat_f64_u);
            Expect(std::get<Tokens::NumericInstr>(lexing("i64.trunc_sat_f32_s")).value == Tokens::NumericInstr::I64Trunc_sat_f32_s);
            Expect(std::get<Tokens::NumericInstr>(lexing("i64.trunc_sat_f32_u")).value == Tokens::NumericInstr::I64Trunc_sat_f32_u);
            Expect(std::get<Tokens::NumericInstr>(lexing("i64.trunc_sat_f64_s")).value == Tokens::NumericInstr::I64Trunc_sat_f64_s);
            Expect(std::get<Tokens::NumericInstr>(lexing("i64.trunc_sat_f64_u")).value == Tokens::NumericInstr::I64Trunc_sat_f64_u);
        })
    })
};