// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <harness.hpp>
#include <WasmVM.hpp>
#include <parse/exception.hpp>

using namespace WasmVM;
using namespace Testing;

Suite import {
    Test("func", {
        ParseFile(test_module, "func.wat");
        // types
        FuncType& type0 = test_module.types[0];
        Expect(type0.params.size() == 0);
        Expect(type0.results.size() == 0);
        FuncType& type1 = test_module.types[1];
        Expect(type1.params.size() == 1);
        Expect(type1.params[0] == ValueType::i32);
        Expect(type1.results.size() == 0);
        FuncType& type3 = test_module.types[2];
        Expect(type3.params.size() == 2);
        Expect(type3.params[0] == ValueType::i32);
        Expect(type3.params[1] == ValueType::f32);
        Expect(type3.results.size() == 0);
        FuncType& type4 = test_module.types[3];
        Expect(type4.params.size() == 1);
        Expect(type4.params[0] == ValueType::i32);
        Expect(type4.results.size() == 1);
        Expect(type4.results[0] == ValueType::f64);
        // imports
        WasmImport& imp0 = test_module.imports[0];
        Expect(std::holds_alternative<index_t>(imp0.desc));
        Expect(std::get<index_t>(imp0.desc) == 0);
        WasmImport& imp1 = test_module.imports[1];
        Expect(std::holds_alternative<index_t>(imp1.desc));
        Expect(std::get<index_t>(imp1.desc) == 0);
        WasmImport& imp2 = test_module.imports[2];
        Expect(std::holds_alternative<index_t>(imp2.desc));
        Expect(std::get<index_t>(imp2.desc) == 1);
        WasmImport& imp3 = test_module.imports[3];
        Expect(std::holds_alternative<index_t>(imp3.desc));
        Expect(std::get<index_t>(imp3.desc) == 2);
        WasmImport& imp4 = test_module.imports[4];
        Expect(std::holds_alternative<index_t>(imp4.desc));
        Expect(std::get<index_t>(imp4.desc) == 3);
    })
    Test("table", {
        ParseFile(test_module, "table.wat");
        WasmImport& imp0 = test_module.imports[0];
        Expect(std::holds_alternative<TableType>(imp0.desc));
        TableType& table0 = std::get<TableType>(imp0.desc);
        Expect(table0.limits.min == 2);
        Expect(table0.limits.max && table0.limits.max.value() == 3);
        Expect(table0.reftype == RefType::funcref);
        WasmImport& imp1 = test_module.imports[1];
        Expect(std::holds_alternative<TableType>(imp1.desc));
        TableType& table1 = std::get<TableType>(imp1.desc);
        Expect(table1.limits.min == 4);
        Expect(table1.limits.max && table1.limits.max.value() == 5);
        Expect(table1.reftype == RefType::externref);
        WasmImport& imp2 = test_module.imports[2];
        Expect(std::holds_alternative<TableType>(imp2.desc));
        TableType& table2 = std::get<TableType>(imp2.desc);
        Expect(table2.limits.min == 6);
        Expect(!table2.limits.max);
        Expect(table2.reftype == RefType::funcref);
    })
    Test("memory", {
        ParseFile(test_module, "memory.wat");
        WasmImport& imp0 = test_module.imports[0];
        Expect(std::holds_alternative<MemType>(imp0.desc));
        MemType& mem0 = std::get<MemType>(imp0.desc);
        Expect(mem0.min == 2);
        Expect(mem0.max && mem0.max.value() == 3);
        WasmImport& imp1 = test_module.imports[1];
        Expect(std::holds_alternative<MemType>(imp1.desc));
        MemType& mem1 = std::get<MemType>(imp1.desc);
        Expect(mem1.min == 6);
        Expect(!mem1.max);
    })
    Test("global", {
        ParseFile(test_module, "global.wat");
        WasmImport& imp0 = test_module.imports[0];
        Expect(std::holds_alternative<GlobalType>(imp0.desc));
        GlobalType& glob0 = std::get<GlobalType>(imp0.desc);
        Expect(glob0.mut == GlobalType::Mut::constant);
        Expect(glob0.type == ValueType::i64);
        WasmImport& imp1 = test_module.imports[1];
        Expect(std::holds_alternative<GlobalType>(imp1.desc));
        GlobalType& glob1 = std::get<GlobalType>(imp1.desc);
        Expect(glob1.mut == GlobalType::Mut::variable);
        Expect(glob1.type == ValueType::f32);
    })
};