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

    })
    Test("parametric", {

    })
    Test("variable", {

    })
    Test("table", {

    })
    Test("memory", {

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