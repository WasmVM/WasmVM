// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <harness.hpp>
#include <WasmVM.hpp>
#include <exception.hpp>

using namespace WasmVM;
using namespace Testing;

Suite elem {
    Test("passive", {
        ParseFile(test_module, "passive.wat");

        WasmElem& elem0 = test_module.elems[0];
        Expect(elem0.type == RefType::funcref);
        Expect(elem0.mode.type == WasmElem::ElemMode::Mode::passive);
        Expect(elem0.elemlist.empty());

        WasmElem& elem1 = test_module.elems[1];
        Expect(elem1.type == RefType::externref);
        Expect(elem1.mode.type == WasmElem::ElemMode::Mode::passive);
        Expect(elem1.elemlist.empty());

        WasmElem& elem2 = test_module.elems[2];
        Expect(elem2.type == RefType::funcref);
        Expect(elem2.mode.type == WasmElem::ElemMode::Mode::passive);
        Expect(elem2.elemlist.size() == 1);
        Instr::Ref_func& instr2 = std::get<Instr::Ref_func>(elem2.elemlist[0]);
        Expect(instr2.index == 2);

        WasmElem& elem3 = test_module.elems[3];
        Expect(elem3.type == RefType::funcref);
        Expect(elem3.mode.type == WasmElem::ElemMode::Mode::passive);
        Expect(elem3.elemlist.size() == 2);
        Instr::Ref_func& instr3_0 = std::get<Instr::Ref_func>(elem3.elemlist[0]);
        Expect(instr3_0.index == 3);
        Instr::Ref_func& instr3_1 = std::get<Instr::Ref_func>(elem3.elemlist[1]);
        Expect(instr3_1.index == 4);

        WasmElem& elem4 = test_module.elems[4];
        Expect(elem4.type == RefType::funcref);
        Expect(elem4.mode.type == WasmElem::ElemMode::Mode::passive);
        Expect(elem4.elemlist.size() == 1);
        Instr::Ref_func& instr4 = std::get<Instr::Ref_func>(elem4.elemlist[0]);
        Expect(instr4.index == 5);

        WasmElem& elem5 = test_module.elems[5];
        Expect(elem5.type == RefType::funcref);
        Expect(elem5.mode.type == WasmElem::ElemMode::Mode::passive);
        Expect(elem5.elemlist.size() == 2);
        Instr::Ref_func& instr5_0 = std::get<Instr::Ref_func>(elem5.elemlist[0]);
        Expect(instr5_0.index == 6);
        Instr::Ref_func& instr5_1 = std::get<Instr::Ref_func>(elem5.elemlist[1]);
        Expect(instr5_1.index == 7);
    })
    Test("active", {
        ParseFile(test_module, "active.wat");

        WasmElem& elem0 = test_module.elems[0];
        Expect(elem0.type == RefType::funcref);
        Expect(elem0.mode.type == WasmElem::ElemMode::Mode::active);
        Expect(elem0.mode.tableidx.value() == 1);
        Expect(std::get<Instr::I32_const>(elem0.mode.offset.value()).value == 2);
        Expect(elem0.elemlist.size() == 1);
        Instr::Ref_func& instr0 = std::get<Instr::Ref_func>(elem0.elemlist[0]);
        Expect(instr0.index == 3);

        WasmElem& elem1 = test_module.elems[1];
        Expect(elem1.type == RefType::funcref);
        Expect(elem1.mode.type == WasmElem::ElemMode::Mode::active);
        Expect(elem1.mode.tableidx.value() == 5);
        Expect(std::get<Instr::I32_const>(elem1.mode.offset.value()).value == 6);
        Expect(elem1.elemlist.empty());

        WasmElem& elem2 = test_module.elems[2];
        Expect(elem2.type == RefType::funcref);
        Expect(elem2.mode.type == WasmElem::ElemMode::Mode::active);
        Expect(elem2.mode.tableidx.value() == 7);
        Expect(std::get<Instr::I32_const>(elem2.mode.offset.value()).value == 8);
        Expect(elem2.elemlist.empty());

        WasmElem& elem3 = test_module.elems[3];
        Expect(elem3.type == RefType::funcref);
        Expect(elem3.mode.type == WasmElem::ElemMode::Mode::active);
        Expect(elem3.mode.tableidx.value() == 0);
        Expect(std::get<Instr::I32_const>(elem3.mode.offset.value()).value == 9);
        Expect(elem3.elemlist.size() == 1);
        Instr::Ref_func& instr3 = std::get<Instr::Ref_func>(elem3.elemlist[0]);
        Expect(instr3.index == 10);

        WasmElem& elem4 = test_module.elems[4];
        Expect(elem4.type == RefType::funcref);
        Expect(elem4.mode.type == WasmElem::ElemMode::Mode::active);
        Expect(elem4.mode.tableidx.value() == 0);
        Expect(std::get<Instr::I32_const>(elem4.mode.offset.value()).value == 11);
        Expect(elem4.elemlist.size() == 2);
        Instr::Ref_func& instr4_0 = std::get<Instr::Ref_func>(elem4.elemlist[0]);
        Expect(instr4_0.index == 12);
        Instr::Ref_func& instr4_1 = std::get<Instr::Ref_func>(elem4.elemlist[1]);
        Expect(instr4_1.index == 13);

    })
    Test("declarative", {
        ParseFile(test_module, "declarative.wat");

        WasmElem& elem0 = test_module.elems[0];
        Expect(elem0.type == RefType::funcref);
        Expect(elem0.mode.type == WasmElem::ElemMode::Mode::declarative);
        Expect(elem0.elemlist.empty());

        WasmElem& elem1 = test_module.elems[1];
        Expect(elem1.type == RefType::funcref);
        Expect(elem1.mode.type == WasmElem::ElemMode::Mode::declarative);
        Expect(elem1.elemlist.empty());
    })
};