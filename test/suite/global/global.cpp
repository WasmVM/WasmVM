// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <harness.hpp>
#include <WasmVM.hpp>
#include <exception.hpp>

using namespace WasmVM;
using namespace Testing;

Suite global {
    Test("normal", {
        ParseFile(test_module, "normal.wat");
        WasmGlobal& glo0 = test_module.globals[0];
        Expect(glo0.type.type == ValueType::i64);
        Expect(glo0.type.mut == GlobalType::constant);
        Expect(std::holds_alternative<Instr::I64_const>(glo0.init));
        Expect(std::get<Instr::I64_const>(glo0.init).value == 5);
        WasmGlobal& glo1 = test_module.globals[1];
        Expect(glo1.type.type == ValueType::f32);
        Expect(glo1.type.mut == GlobalType::variable);
        Expect(std::holds_alternative<Instr::F32_const>(glo1.init));
        Expect(std::get<Instr::F32_const>(glo1.init).value == 2.8f);
        WasmGlobal& glo2 = test_module.globals[2];
        Expect(glo2.type.type == ValueType::funcref);
        Expect(glo2.type.mut == GlobalType::constant);
        Expect(std::holds_alternative<Instr::Ref_null>(glo2.init));
        Expect(std::get<Instr::Ref_null>(glo2.init).heaptype == RefType::funcref);
    })
    Test("with_data", {
        // TODO:
    })
    // Test("with_import", {
    //     ParseFile(test_module, "with_import.wat");

    //     WasmImport& imp0 = test_module.imports[0];
    //     Expect(std::holds_alternative<MemType>(imp0.desc));
    //     MemType& mem0 = std::get<MemType>(imp0.desc);
    //     Expect(imp0.module == "testmod");
    //     Expect(imp0.name == "mem0");
    //     Expect(mem0.min == 12);
    //     Expect(mem0.max.has_value() == false);

    //     WasmImport& imp1 = test_module.imports[1];
    //     Expect(std::holds_alternative<MemType>(imp1.desc));
    //     MemType& mem1 = std::get<MemType>(imp1.desc);
    //     Expect(imp1.module == "testmod");
    //     Expect(imp1.name == "mem1");
    //     Expect(mem1.min == 13);
    //     Expect(mem1.max.has_value() == false);

    //     WasmImport& imp2 = test_module.imports[2];
    //     Expect(std::holds_alternative<MemType>(imp2.desc));
    //     MemType& mem2 = std::get<MemType>(imp2.desc);
    //     Expect(imp2.module == "testmod");
    //     Expect(imp2.name == "mem3");
    //     Expect(mem2.min == 15);
    //     Expect(mem2.max.has_value() == false);

    //     MemType& mem3 = test_module.mems[0];
    //     Expect(mem3.min == 14);
    //     Expect(mem3.max.has_value() == false);
    // })
    Test("with_export", {
        // TODO:
    })
};