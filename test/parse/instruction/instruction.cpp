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

    })
};