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
    Test("with_import", {
        ParseFile(test_module, "with_import.wat");
        WasmImport& imp0 = test_module.imports[0];
        Expect(std::holds_alternative<GlobalType>(imp0.desc));
        GlobalType& glo0 = std::get<GlobalType>(imp0.desc);
        Expect(imp0.module == "testmod");
        Expect(imp0.name == "glo0");
        Expect(glo0.type == ValueType::externref);
        Expect(glo0.mut == GlobalType::constant);

        WasmImport& imp1 = test_module.imports[1];
        Expect(std::holds_alternative<GlobalType>(imp1.desc));
        GlobalType& glo1 = std::get<GlobalType>(imp1.desc);
        Expect(imp1.module == "testmod");
        Expect(imp1.name == "glo1");
        Expect(glo1.type == ValueType::f32);
        Expect(glo1.mut == GlobalType::constant);

        WasmImport& imp2 = test_module.imports[2];
        Expect(std::holds_alternative<GlobalType>(imp2.desc));
        GlobalType& glo2 = std::get<GlobalType>(imp2.desc);
        Expect(imp2.module == "testmod");
        Expect(imp2.name == "glo3");
        Expect(glo2.type == ValueType::f64);
        Expect(glo2.mut == GlobalType::variable);

        WasmGlobal& glo3 = test_module.globals[0];
        Expect(glo3.type.type == ValueType::i32);
        Expect(glo3.type.mut == GlobalType::constant);
        Expect(std::holds_alternative<Instr::I32_const>(glo3.init));
        Expect(std::get<Instr::I32_const>(glo3.init).value == 3);
    })
    Test("with_export", {
        // TODO:
    })
};