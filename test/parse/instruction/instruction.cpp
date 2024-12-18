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
            ParseFile(test_module, "i32.wat");
            std::vector<WasmInstr>& instrs = test_module.funcs[0].body;
            Expect(std::holds_alternative<Instr::I32_const>(instrs[0]));
            Expect(std::get<Instr::I32_const>(instrs[0]).value == 2);
            Expect(std::holds_alternative<Instr::I32_clz>(instrs[1]));
            Expect(std::holds_alternative<Instr::I32_ctz>(instrs[2]));
            Expect(std::holds_alternative<Instr::I32_popcnt>(instrs[3]));
            Expect(std::holds_alternative<Instr::I32_add>(instrs[4]));
            Expect(std::holds_alternative<Instr::I32_sub>(instrs[5]));
            Expect(std::holds_alternative<Instr::I32_mul>(instrs[6]));
            Expect(std::holds_alternative<Instr::I32_div_s>(instrs[7]));
            Expect(std::holds_alternative<Instr::I32_div_u>(instrs[8]));
            Expect(std::holds_alternative<Instr::I32_rem_s>(instrs[9]));
            Expect(std::holds_alternative<Instr::I32_rem_u>(instrs[10]));
            Expect(std::holds_alternative<Instr::I32_and>(instrs[11]));
            Expect(std::holds_alternative<Instr::I32_or>(instrs[12]));
            Expect(std::holds_alternative<Instr::I32_xor>(instrs[13]));
            Expect(std::holds_alternative<Instr::I32_shl>(instrs[14]));
            Expect(std::holds_alternative<Instr::I32_shr_s>(instrs[15]));
            Expect(std::holds_alternative<Instr::I32_shr_u>(instrs[16]));
            Expect(std::holds_alternative<Instr::I32_rotl>(instrs[17]));
            Expect(std::holds_alternative<Instr::I32_rotr>(instrs[18]));
            Expect(std::holds_alternative<Instr::I32_eqz>(instrs[19]));
            Expect(std::holds_alternative<Instr::I32_eq>(instrs[20]));
            Expect(std::holds_alternative<Instr::I32_ne>(instrs[21]));
            Expect(std::holds_alternative<Instr::I32_lt_s>(instrs[22]));
            Expect(std::holds_alternative<Instr::I32_lt_u>(instrs[23]));
            Expect(std::holds_alternative<Instr::I32_gt_s>(instrs[24]));
            Expect(std::holds_alternative<Instr::I32_gt_u>(instrs[25]));
            Expect(std::holds_alternative<Instr::I32_le_s>(instrs[26]));
            Expect(std::holds_alternative<Instr::I32_le_u>(instrs[27]));
            Expect(std::holds_alternative<Instr::I32_ge_s>(instrs[28]));
            Expect(std::holds_alternative<Instr::I32_ge_u>(instrs[29]));
            Expect(std::holds_alternative<Instr::I32_wrap_i64>(instrs[30]));
            Expect(std::holds_alternative<Instr::I32_trunc_f32_s>(instrs[31]));
            Expect(std::holds_alternative<Instr::I32_trunc_f32_u>(instrs[32]));
            Expect(std::holds_alternative<Instr::I32_trunc_f64_s>(instrs[33]));
            Expect(std::holds_alternative<Instr::I32_trunc_f64_u>(instrs[34]));
            Expect(std::holds_alternative<Instr::I32_trunc_sat_f32_s>(instrs[35]));
            Expect(std::holds_alternative<Instr::I32_trunc_sat_f32_u>(instrs[36]));
            Expect(std::holds_alternative<Instr::I32_trunc_sat_f64_s>(instrs[37]));
            Expect(std::holds_alternative<Instr::I32_trunc_sat_f64_u>(instrs[38]));
            Expect(std::holds_alternative<Instr::I32_reinterpret_f32>(instrs[39]));
            Expect(std::holds_alternative<Instr::I32_extend8_s>(instrs[40]));
            Expect(std::holds_alternative<Instr::I32_extend16_s>(instrs[41]));
        })
        Test("i64", {
            ParseFile(test_module, "i64.wat");
            std::vector<WasmInstr>& instrs = test_module.funcs[0].body;
            Expect(std::holds_alternative<Instr::I64_const>(instrs[0]));
            Expect(std::get<Instr::I64_const>(instrs[0]).value == 2);
            Expect(std::holds_alternative<Instr::I64_clz>(instrs[1]));
		    Expect(std::holds_alternative<Instr::I64_ctz>(instrs[2]));
		    Expect(std::holds_alternative<Instr::I64_popcnt>(instrs[3]));
		    Expect(std::holds_alternative<Instr::I64_add>(instrs[4]));
		    Expect(std::holds_alternative<Instr::I64_sub>(instrs[5]));
		    Expect(std::holds_alternative<Instr::I64_mul>(instrs[6]));
		    Expect(std::holds_alternative<Instr::I64_div_s>(instrs[7]));
		    Expect(std::holds_alternative<Instr::I64_div_u>(instrs[8]));
		    Expect(std::holds_alternative<Instr::I64_rem_s>(instrs[9]));
		    Expect(std::holds_alternative<Instr::I64_rem_u>(instrs[10]));
		    Expect(std::holds_alternative<Instr::I64_and>(instrs[11]));
		    Expect(std::holds_alternative<Instr::I64_or>(instrs[12]));
		    Expect(std::holds_alternative<Instr::I64_xor>(instrs[13]));
		    Expect(std::holds_alternative<Instr::I64_shl>(instrs[14]));
		    Expect(std::holds_alternative<Instr::I64_shr_s>(instrs[15]));
		    Expect(std::holds_alternative<Instr::I64_shr_u>(instrs[16]));
		    Expect(std::holds_alternative<Instr::I64_rotl>(instrs[17]));
		    Expect(std::holds_alternative<Instr::I64_rotr>(instrs[18]));
            Expect(std::holds_alternative<Instr::I64_eqz>(instrs[19]));
		    Expect(std::holds_alternative<Instr::I64_eq>(instrs[20]));
		    Expect(std::holds_alternative<Instr::I64_ne>(instrs[21]));
		    Expect(std::holds_alternative<Instr::I64_lt_s>(instrs[22]));
		    Expect(std::holds_alternative<Instr::I64_lt_u>(instrs[23]));
		    Expect(std::holds_alternative<Instr::I64_gt_s>(instrs[24]));
		    Expect(std::holds_alternative<Instr::I64_gt_u>(instrs[25]));
		    Expect(std::holds_alternative<Instr::I64_le_s>(instrs[26]));
		    Expect(std::holds_alternative<Instr::I64_le_u>(instrs[27]));
		    Expect(std::holds_alternative<Instr::I64_ge_s>(instrs[28]));
		    Expect(std::holds_alternative<Instr::I64_ge_u>(instrs[29]));
		    Expect(std::holds_alternative<Instr::I64_extend_i32_s>(instrs[30]));
		    Expect(std::holds_alternative<Instr::I64_extend_i32_u>(instrs[31]));
		    Expect(std::holds_alternative<Instr::I64_trunc_f32_s>(instrs[32]));
		    Expect(std::holds_alternative<Instr::I64_trunc_f32_u>(instrs[33]));
		    Expect(std::holds_alternative<Instr::I64_trunc_f64_s>(instrs[34]));
		    Expect(std::holds_alternative<Instr::I64_trunc_f64_u>(instrs[35]));
		    Expect(std::holds_alternative<Instr::I64_trunc_sat_f32_s>(instrs[36]));
		    Expect(std::holds_alternative<Instr::I64_trunc_sat_f32_u>(instrs[37]));
		    Expect(std::holds_alternative<Instr::I64_trunc_sat_f64_s>(instrs[38]));
		    Expect(std::holds_alternative<Instr::I64_trunc_sat_f64_u>(instrs[39]));
		    Expect(std::holds_alternative<Instr::I64_extend8_s>(instrs[40]));
		    Expect(std::holds_alternative<Instr::I64_extend16_s>(instrs[41]));
		    Expect(std::holds_alternative<Instr::I64_extend32_s>(instrs[42]));
        })
        Test("f32", {
            ParseFile(test_module, "f32.wat");
            std::vector<WasmInstr>& instrs = test_module.funcs[0].body;
            Expect(std::holds_alternative<Instr::F32_const>(instrs[0]));
            Expect(std::get<Instr::F32_const>(instrs[0]).value == 3.14f);
            Expect(std::holds_alternative<Instr::F32_abs>(instrs[1]));
            Expect(std::holds_alternative<Instr::F32_neg>(instrs[2]));
            Expect(std::holds_alternative<Instr::F32_ceil>(instrs[3]));
            Expect(std::holds_alternative<Instr::F32_floor>(instrs[4]));
            Expect(std::holds_alternative<Instr::F32_trunc>(instrs[5]));
            Expect(std::holds_alternative<Instr::F32_nearest>(instrs[6]));
            Expect(std::holds_alternative<Instr::F32_sqrt>(instrs[7]));
            Expect(std::holds_alternative<Instr::F32_add>(instrs[8]));
            Expect(std::holds_alternative<Instr::F32_sub>(instrs[9]));
            Expect(std::holds_alternative<Instr::F32_mul>(instrs[10]));
            Expect(std::holds_alternative<Instr::F32_div>(instrs[11]));
            Expect(std::holds_alternative<Instr::F32_min>(instrs[12]));
            Expect(std::holds_alternative<Instr::F32_max>(instrs[13]));
            Expect(std::holds_alternative<Instr::F32_copysign>(instrs[14]));
            Expect(std::holds_alternative<Instr::F32_eq>(instrs[15]));
            Expect(std::holds_alternative<Instr::F32_ne>(instrs[16]));
            Expect(std::holds_alternative<Instr::F32_lt>(instrs[17]));
            Expect(std::holds_alternative<Instr::F32_gt>(instrs[18]));
            Expect(std::holds_alternative<Instr::F32_le>(instrs[19]));
            Expect(std::holds_alternative<Instr::F32_ge>(instrs[20]));
            Expect(std::holds_alternative<Instr::F32_convert_i32_s>(instrs[21]));
            Expect(std::holds_alternative<Instr::F32_convert_i32_u>(instrs[22]));
            Expect(std::holds_alternative<Instr::F32_convert_i64_s>(instrs[23]));
            Expect(std::holds_alternative<Instr::F32_convert_i64_u>(instrs[24]));
            Expect(std::holds_alternative<Instr::F32_demote_f64>(instrs[25]));
        })
        Test("f64", {
            ParseFile(test_module, "f64.wat");
            std::vector<WasmInstr>& instrs = test_module.funcs[0].body;
            Expect(std::holds_alternative<Instr::F64_const>(instrs[0]));
            Expect(std::get<Instr::F64_const>(instrs[0]).value == 3.14);
            Expect(std::holds_alternative<Instr::F64_abs>(instrs[1]));
            Expect(std::holds_alternative<Instr::F64_neg>(instrs[2]));
            Expect(std::holds_alternative<Instr::F64_ceil>(instrs[3]));
            Expect(std::holds_alternative<Instr::F64_floor>(instrs[4]));
            Expect(std::holds_alternative<Instr::F64_trunc>(instrs[5]));
            Expect(std::holds_alternative<Instr::F64_nearest>(instrs[6]));
            Expect(std::holds_alternative<Instr::F64_sqrt>(instrs[7]));
            Expect(std::holds_alternative<Instr::F64_add>(instrs[8]));
            Expect(std::holds_alternative<Instr::F64_sub>(instrs[9]));
            Expect(std::holds_alternative<Instr::F64_mul>(instrs[10]));
            Expect(std::holds_alternative<Instr::F64_div>(instrs[11]));
            Expect(std::holds_alternative<Instr::F64_min>(instrs[12]));
            Expect(std::holds_alternative<Instr::F64_max>(instrs[13]));
            Expect(std::holds_alternative<Instr::F64_copysign>(instrs[14]));
            Expect(std::holds_alternative<Instr::F64_eq>(instrs[15]));
            Expect(std::holds_alternative<Instr::F64_ne>(instrs[16]));
            Expect(std::holds_alternative<Instr::F64_lt>(instrs[17]));
            Expect(std::holds_alternative<Instr::F64_gt>(instrs[18]));
            Expect(std::holds_alternative<Instr::F64_le>(instrs[19]));
            Expect(std::holds_alternative<Instr::F64_ge>(instrs[20]));
            Expect(std::holds_alternative<Instr::F64_convert_i32_s>(instrs[21]));
            Expect(std::holds_alternative<Instr::F64_convert_i32_u>(instrs[22]));
            Expect(std::holds_alternative<Instr::F64_convert_i64_s>(instrs[23]));
            Expect(std::holds_alternative<Instr::F64_convert_i64_u>(instrs[24]));
            Expect(std::holds_alternative<Instr::F64_promote_f32>(instrs[25]));
            Expect(std::holds_alternative<Instr::F64_reinterpret_i64>(instrs[26]));
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