// Copyright 2024 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <harness.hpp>
#include <WasmVM.hpp>
#include <exception.hpp>

using namespace WasmVM;
using namespace Testing;

Suite instruction {
    Test("control", {
        ParseFile(test_module, "control.wat");
        std::vector<WasmInstr>& instrs = test_module.funcs[0].body;
        // unreachable
        Expect(std::holds_alternative<Instr::Unreachable>(instrs[0]));
        // nop
        Expect(std::holds_alternative<Instr::Nop>(instrs[1]));
        // return
        Expect(std::holds_alternative<Instr::Return>(instrs[2]));
        // br
        Expect(std::holds_alternative<Instr::Br>(instrs[3]));
        Expect(std::get<Instr::Br>(instrs[3]).index == 1);
        // br_if
        Expect(std::holds_alternative<Instr::Br_if>(instrs[4]));
        Expect(std::get<Instr::Br_if>(instrs[4]).index == 2);
        // br_table
        Instr::Br_table br_table = std::get<Instr::Br_table>(instrs[5]);
        Expect(br_table.indices.size() == 3);
        Expect(br_table.indices[0] == 3);
        Expect(br_table.indices[1] == 4);
        Expect(br_table.indices[2] == 5);
        // call
        Expect(std::holds_alternative<Instr::Call>(instrs[6]));
        Expect(std::get<Instr::Call>(instrs[6]).index == 6);
        // call_indirect
        Instr::Call_indirect call_indirect0 = std::get<Instr::Call_indirect>(instrs[7]);
        Expect(call_indirect0.tableidx == 7);
        Expect(call_indirect0.typeidx == 8);
        Instr::Call_indirect call_indirect1 = std::get<Instr::Call_indirect>(instrs[8]);
        Expect(call_indirect1.tableidx == 0);
        Expect(call_indirect1.typeidx == 9);
    })
    Test("block", {
        ParseFile(test_module, "block.wat");
        std::vector<WasmInstr>& instrs = test_module.funcs[0].body;
        // block with id
        Expect(std::holds_alternative<Instr::Block>(instrs[0]));
        Expect(std::get<Instr::Block>(instrs[0]).type.has_value() == false);
        Expect(std::holds_alternative<Instr::Nop>(instrs[1]));
        Expect(std::holds_alternative<Instr::End>(instrs[2]));
        // block with result type
        Expect(std::holds_alternative<Instr::Block>(instrs[3]));
        Expect(std::get<Instr::Block>(instrs[3]).type.value() == 1);
        Expect(std::holds_alternative<Instr::Nop>(instrs[4]));
        Expect(std::holds_alternative<Instr::End>(instrs[5]));
        // block with typeuse
        Expect(std::holds_alternative<Instr::Block>(instrs[6]));
        Expect(std::get<Instr::Block>(instrs[6]).type.value() == 0);
        Expect(std::holds_alternative<Instr::Nop>(instrs[7]));
        Expect(std::holds_alternative<Instr::End>(instrs[8]));
        // loop
        Expect(std::holds_alternative<Instr::Loop>(instrs[9]));
        Expect(std::get<Instr::Loop>(instrs[9]).type.has_value() == false);
        Expect(std::holds_alternative<Instr::Nop>(instrs[10]));
        Expect(std::holds_alternative<Instr::End>(instrs[11]));
        // if then else
        Expect(std::holds_alternative<Instr::If>(instrs[12]));
        Expect(std::holds_alternative<Instr::Unreachable>(instrs[13]));
        Expect(std::holds_alternative<Instr::Else>(instrs[14]));
        Expect(std::holds_alternative<Instr::Nop>(instrs[15]));
        Expect(std::holds_alternative<Instr::End>(instrs[16]));
        // if else
        Expect(std::holds_alternative<Instr::If>(instrs[17]));
        Expect(std::holds_alternative<Instr::Unreachable>(instrs[18]));
        Expect(std::holds_alternative<Instr::Else>(instrs[19]));
        Expect(std::holds_alternative<Instr::End>(instrs[20]));
        // nested
        Expect(std::holds_alternative<Instr::Loop>(instrs[21]));
        Expect(std::holds_alternative<Instr::Block>(instrs[22]));
        Expect(std::holds_alternative<Instr::Unreachable>(instrs[23]));
        Expect(std::holds_alternative<Instr::End>(instrs[24]));
        Expect(std::holds_alternative<Instr::Block>(instrs[25]));
        Expect(std::holds_alternative<Instr::Nop>(instrs[26]));
        Expect(std::holds_alternative<Instr::End>(instrs[27]));
        Expect(std::holds_alternative<Instr::End>(instrs[28]));
    })
    Test("reference", {
        ParseFile(test_module, "reference.wat");
        std::vector<WasmInstr>& instrs = test_module.funcs[1].body;
        Expect(std::holds_alternative<Instr::Ref_null>(instrs[0]));
        Expect(std::get<Instr::Ref_null>(instrs[0]).heaptype == RefType::funcref);
        Expect(std::holds_alternative<Instr::Ref_null>(instrs[1]));
        Expect(std::get<Instr::Ref_null>(instrs[1]).heaptype == RefType::externref);
        Expect(std::holds_alternative<Instr::Ref_is_null>(instrs[2]));
        Expect(std::holds_alternative<Instr::Ref_func>(instrs[3]));
        Expect(std::get<Instr::Ref_func>(instrs[3]).index == 0);
        Expect(std::holds_alternative<Instr::Ref_func>(instrs[4]));
        Expect(std::get<Instr::Ref_func>(instrs[4]).index == 1);
    })
    Test("parametric", {
        ParseFile(test_module, "parametric.wat");
        std::vector<WasmInstr>& instrs = test_module.funcs[0].body;
        Expect(std::holds_alternative<Instr::Select>(instrs[0]));
        Expect(std::get<Instr::Select>(instrs[0]).valtypes.empty());
        Expect(std::holds_alternative<Instr::Select>(instrs[1]));
        Expect(std::get<Instr::Select>(instrs[1]).valtypes.size() == 2);
        Expect(std::get<Instr::Select>(instrs[1]).valtypes[0] == ValueType::i32);
        Expect(std::get<Instr::Select>(instrs[1]).valtypes[1] == ValueType::i64);
        Expect(std::holds_alternative<Instr::Drop>(instrs[2]));
    })
    Test("variable", {
        ParseFile(test_module, "variable.wat");
        std::vector<WasmInstr>& instrs = test_module.funcs[0].body;
        Expect(std::holds_alternative<Instr::Local_get>(instrs[0]));
        Expect(std::get<Instr::Local_get>(instrs[0]).index == 0);
        Expect(std::holds_alternative<Instr::Local_get>(instrs[1]));
        Expect(std::get<Instr::Local_get>(instrs[1]).index == 1);
        Expect(std::holds_alternative<Instr::Local_set>(instrs[2]));
        Expect(std::get<Instr::Local_set>(instrs[2]).index == 0);
        Expect(std::holds_alternative<Instr::Local_tee>(instrs[3]));
        Expect(std::get<Instr::Local_tee>(instrs[3]).index == 0);
        Expect(std::holds_alternative<Instr::Global_get>(instrs[4]));
        Expect(std::get<Instr::Global_get>(instrs[4]).index == 0);
        Expect(std::holds_alternative<Instr::Global_set>(instrs[5]));
        Expect(std::get<Instr::Global_set>(instrs[5]).index == 0);
    })
    Test("table", {
        ParseFile(test_module, "table.wat");
        std::vector<WasmInstr>& instrs = test_module.funcs[0].body;
        Expect(std::holds_alternative<Instr::Table_get>(instrs[0]));
        Expect(std::get<Instr::Table_get>(instrs[0]).index == 1);
        Expect(std::holds_alternative<Instr::Table_get>(instrs[1]));
        Expect(std::get<Instr::Table_get>(instrs[1]).index == 0);
        Expect(std::holds_alternative<Instr::Table_set>(instrs[2]));
        Expect(std::get<Instr::Table_set>(instrs[2]).index == 1);
        Expect(std::holds_alternative<Instr::Table_set>(instrs[3]));
        Expect(std::get<Instr::Table_set>(instrs[3]).index == 0);
        Expect(std::holds_alternative<Instr::Table_size>(instrs[4]));
        Expect(std::get<Instr::Table_size>(instrs[4]).index == 1);
        Expect(std::holds_alternative<Instr::Table_size>(instrs[5]));
        Expect(std::get<Instr::Table_size>(instrs[5]).index == 0);
        Expect(std::holds_alternative<Instr::Table_grow>(instrs[6]));
        Expect(std::get<Instr::Table_grow>(instrs[6]).index == 1);
        Expect(std::holds_alternative<Instr::Table_grow>(instrs[7]));
        Expect(std::get<Instr::Table_grow>(instrs[7]).index == 0);
        Expect(std::holds_alternative<Instr::Table_fill>(instrs[8]));
        Expect(std::get<Instr::Table_fill>(instrs[8]).index == 1);
        Expect(std::holds_alternative<Instr::Table_fill>(instrs[9]));
        Expect(std::get<Instr::Table_fill>(instrs[9]).index == 0);
        Expect(std::holds_alternative<Instr::Table_copy>(instrs[10]));
        Expect(std::get<Instr::Table_copy>(instrs[10]).dstidx == 1);
        Expect(std::get<Instr::Table_copy>(instrs[10]).srcidx == 2);
        Expect(std::holds_alternative<Instr::Table_copy>(instrs[11]));
        Expect(std::get<Instr::Table_copy>(instrs[11]).dstidx == 0);
        Expect(std::get<Instr::Table_copy>(instrs[11]).srcidx == 0);
        Expect(std::holds_alternative<Instr::Table_init>(instrs[12]));
        Expect(std::get<Instr::Table_init>(instrs[12]).tableidx == 1);
        Expect(std::get<Instr::Table_init>(instrs[12]).elemidx == 3);
        Expect(std::holds_alternative<Instr::Table_init>(instrs[13]));
        Expect(std::get<Instr::Table_init>(instrs[13]).tableidx == 0);
        Expect(std::get<Instr::Table_init>(instrs[13]).elemidx == 3);
        Expect(std::holds_alternative<Instr::Elem_drop>(instrs[14]));
        Expect(std::get<Instr::Elem_drop>(instrs[14]).index == 4);
    })
    Test("memory", {
        ParseFile(test_module, "memory.wat");
        std::vector<WasmInstr>& instrs = test_module.funcs[0].body;
        // i32
        Expect(std::holds_alternative<Instr::I32_load>(instrs[0]));
        Expect(std::get<Instr::I32_load>(instrs[0]).memidx == 0);
        Expect(std::get<Instr::I32_load>(instrs[0]).offset == 4);
        Expect(std::get<Instr::I32_load>(instrs[0]).align == 2);
        Expect(std::holds_alternative<Instr::I32_load8_s>(instrs[1]));
        Expect(std::get<Instr::I32_load8_s>(instrs[1]).memidx == 1);
        Expect(std::get<Instr::I32_load8_s>(instrs[1]).offset == 0);
        Expect(std::get<Instr::I32_load8_s>(instrs[1]).align == 0);
        Expect(std::holds_alternative<Instr::I32_load8_u>(instrs[2]));
        Expect(std::get<Instr::I32_load8_u>(instrs[2]).memidx == 0);
        Expect(std::get<Instr::I32_load8_u>(instrs[2]).offset == 0);
        Expect(std::get<Instr::I32_load8_u>(instrs[2]).align == 0);
        Expect(std::holds_alternative<Instr::I32_load16_s>(instrs[3]));
        Expect(std::get<Instr::I32_load16_s>(instrs[3]).memidx == 0);
        Expect(std::get<Instr::I32_load16_s>(instrs[3]).offset == 0);
        Expect(std::get<Instr::I32_load16_s>(instrs[3]).align == 1);
        Expect(std::holds_alternative<Instr::I32_load16_u>(instrs[4]));
        Expect(std::get<Instr::I32_load16_u>(instrs[4]).memidx == 0);
        Expect(std::get<Instr::I32_load16_u>(instrs[4]).offset == 0);
        Expect(std::get<Instr::I32_load16_u>(instrs[4]).align == 1);
        Expect(std::holds_alternative<Instr::I32_store>(instrs[5]));
        Expect(std::get<Instr::I32_store>(instrs[5]).memidx == 0);
        Expect(std::get<Instr::I32_store>(instrs[5]).offset == 0);
        Expect(std::get<Instr::I32_store>(instrs[5]).align == 2);
        Expect(std::holds_alternative<Instr::I32_store8>(instrs[6]));
        Expect(std::get<Instr::I32_store8>(instrs[6]).memidx == 0);
        Expect(std::get<Instr::I32_store8>(instrs[6]).offset == 0);
        Expect(std::get<Instr::I32_store8>(instrs[6]).align == 0);
        Expect(std::holds_alternative<Instr::I32_store16>(instrs[7]));
        Expect(std::get<Instr::I32_store16>(instrs[7]).memidx == 0);
        Expect(std::get<Instr::I32_store16>(instrs[7]).offset == 0);
        Expect(std::get<Instr::I32_store16>(instrs[7]).align == 1);
        // i64
        Expect(std::holds_alternative<Instr::I64_load>(instrs[8]));
        Expect(std::get<Instr::I64_load>(instrs[8]).memidx == 0);
        Expect(std::get<Instr::I64_load>(instrs[8]).offset == 0);
        Expect(std::get<Instr::I64_load>(instrs[8]).align == 3);
        Expect(std::holds_alternative<Instr::I64_load8_s>(instrs[9]));
        Expect(std::get<Instr::I64_load8_s>(instrs[9]).memidx == 0);
        Expect(std::get<Instr::I64_load8_s>(instrs[9]).offset == 0);
        Expect(std::get<Instr::I64_load8_s>(instrs[9]).align == 0);
        Expect(std::holds_alternative<Instr::I64_load8_u>(instrs[10]));
        Expect(std::get<Instr::I64_load8_u>(instrs[10]).memidx == 0);
        Expect(std::get<Instr::I64_load8_u>(instrs[10]).offset == 0);
        Expect(std::get<Instr::I64_load8_u>(instrs[10]).align == 0);
        Expect(std::holds_alternative<Instr::I64_load16_s>(instrs[11]));
        Expect(std::get<Instr::I64_load16_s>(instrs[11]).memidx == 0);
        Expect(std::get<Instr::I64_load16_s>(instrs[11]).offset == 0);
        Expect(std::get<Instr::I64_load16_s>(instrs[11]).align == 1);
        Expect(std::holds_alternative<Instr::I64_load16_u>(instrs[12]));
        Expect(std::get<Instr::I64_load16_u>(instrs[12]).memidx == 0);
        Expect(std::get<Instr::I64_load16_u>(instrs[12]).offset == 0);
        Expect(std::get<Instr::I64_load16_u>(instrs[12]).align == 1);
        Expect(std::holds_alternative<Instr::I64_load32_s>(instrs[13]));
        Expect(std::get<Instr::I64_load32_s>(instrs[13]).memidx == 0);
        Expect(std::get<Instr::I64_load32_s>(instrs[13]).offset == 0);
        Expect(std::get<Instr::I64_load32_s>(instrs[13]).align == 2);
        Expect(std::holds_alternative<Instr::I64_load32_u>(instrs[14]));
        Expect(std::get<Instr::I64_load32_u>(instrs[14]).memidx == 0);
        Expect(std::get<Instr::I64_load32_u>(instrs[14]).offset == 0);
        Expect(std::get<Instr::I64_load32_u>(instrs[14]).align == 2);
        Expect(std::holds_alternative<Instr::I64_store>(instrs[15]));
        Expect(std::get<Instr::I64_store>(instrs[15]).memidx == 0);
        Expect(std::get<Instr::I64_store>(instrs[15]).offset == 0);
        Expect(std::get<Instr::I64_store>(instrs[15]).align == 3);
        Expect(std::holds_alternative<Instr::I64_store8>(instrs[16]));
        Expect(std::get<Instr::I64_store8>(instrs[16]).memidx == 0);
        Expect(std::get<Instr::I64_store8>(instrs[16]).offset == 0);
        Expect(std::get<Instr::I64_store8>(instrs[16]).align == 0);
        Expect(std::holds_alternative<Instr::I64_store16>(instrs[17]));
        Expect(std::get<Instr::I64_store16>(instrs[17]).memidx == 0);
        Expect(std::get<Instr::I64_store16>(instrs[17]).offset == 0);
        Expect(std::get<Instr::I64_store16>(instrs[17]).align == 1);
        Expect(std::holds_alternative<Instr::I64_store32>(instrs[18]));
        Expect(std::get<Instr::I64_store32>(instrs[18]).memidx == 0);
        Expect(std::get<Instr::I64_store32>(instrs[18]).offset == 0);
        Expect(std::get<Instr::I64_store32>(instrs[18]).align == 2);
        // f32
        Expect(std::holds_alternative<Instr::F32_load>(instrs[19]));
        Expect(std::get<Instr::F32_load>(instrs[19]).memidx == 0);
        Expect(std::get<Instr::F32_load>(instrs[19]).offset == 0);
        Expect(std::get<Instr::F32_load>(instrs[19]).align == 2);
        Expect(std::holds_alternative<Instr::F32_store>(instrs[20]));
        Expect(std::get<Instr::F32_store>(instrs[20]).memidx == 0);
        Expect(std::get<Instr::F32_store>(instrs[20]).offset == 0);
        Expect(std::get<Instr::F32_store>(instrs[20]).align == 2);
        // f64
        Expect(std::holds_alternative<Instr::F64_load>(instrs[21]));
        Expect(std::get<Instr::F64_load>(instrs[21]).memidx == 0);
        Expect(std::get<Instr::F64_load>(instrs[21]).offset == 0);
        Expect(std::get<Instr::F64_load>(instrs[21]).align == 3);
        Expect(std::holds_alternative<Instr::F64_store>(instrs[22]));
        Expect(std::get<Instr::F64_store>(instrs[22]).memidx == 0);
        Expect(std::get<Instr::F64_store>(instrs[22]).offset == 0);
        Expect(std::get<Instr::F64_store>(instrs[22]).align == 3);
    })
    Category("numeric", {
        Test("i32", {

        })
        Test("i64", {

        })
        Test("f32", {

        })
        Test("f64", {

        })
    })
    Test("folded", {
        ParseFile(test_module, "folded.wat");
        std::vector<WasmInstr>& instrs = test_module.funcs[0].body;
        // Single plain
        Expect(std::holds_alternative<Instr::Unreachable>(instrs[0]));
        // plain with folded
        Expect(std::holds_alternative<Instr::Return>(instrs[1]));
        Expect(std::holds_alternative<Instr::Nop>(instrs[2]));
        Expect(std::holds_alternative<Instr::Unreachable>(instrs[3]));
        // block
        Expect(std::holds_alternative<Instr::Block>(instrs[4]));
        Expect(std::holds_alternative<Instr::Nop>(instrs[5]));
        Expect(std::holds_alternative<Instr::End>(instrs[6]));
        // loop
        Expect(std::holds_alternative<Instr::Loop>(instrs[7]));
        Expect(std::holds_alternative<Instr::Unreachable>(instrs[8]));
        Expect(std::holds_alternative<Instr::End>(instrs[9]));
        // if
        Expect(std::holds_alternative<Instr::If>(instrs[10]));
        Expect(std::holds_alternative<Instr::Unreachable>(instrs[11]));
        Expect(std::holds_alternative<Instr::Else>(instrs[12]));
        Expect(std::holds_alternative<Instr::End>(instrs[13]));
        // if then else
        Expect(std::holds_alternative<Instr::If>(instrs[14]));
        Expect(std::holds_alternative<Instr::Unreachable>(instrs[15]));
        Expect(std::holds_alternative<Instr::Else>(instrs[16]));
        Expect(std::holds_alternative<Instr::Nop>(instrs[17]));
        Expect(std::holds_alternative<Instr::End>(instrs[18]));
        // if folded
        Expect(std::holds_alternative<Instr::Return>(instrs[19]));
        Expect(std::holds_alternative<Instr::If>(instrs[20]));
        Expect(std::holds_alternative<Instr::Unreachable>(instrs[21]));
        Expect(std::holds_alternative<Instr::Else>(instrs[22]));
        Expect(std::holds_alternative<Instr::Nop>(instrs[23]));
        Expect(std::holds_alternative<Instr::End>(instrs[24]));
    })
};