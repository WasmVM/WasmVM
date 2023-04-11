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
        std::cout << module_dump(test_module) << std::endl;
        // types
        FuncType& type0 = test_module.types[0];
        Expect(type0.params.size() == 0);
        Expect(type0.results.size() == 0);
        FuncType& type1 = test_module.types[1];
        Expect(type1.params.size() == 1);
        Expect(type1.params[0] == ValueType::i32);
        Expect(type1.results.size() == 0);
        FuncType& type2 = test_module.types[2];
        Expect(type2.params.size() == 0);
        Expect(type2.results.size() == 0);
        FuncType& type3 = test_module.types[3];
        Expect(type3.params.size() == 2);
        Expect(type3.params[0] == ValueType::i32);
        Expect(type3.params[1] == ValueType::f32);
        Expect(type3.results.size() == 0);
        FuncType& type4 = test_module.types[4];
        Expect(type4.params.size() == 1);
        Expect(type4.params[0] == ValueType::i32);
        Expect(type4.results.size() == 1);
        Expect(type4.results[0] == ValueType::f64);
        // imports
        WasmImport& imp0 = test_module.imports[0];
        Expect(std::holds_alternative<index_t>(imp0.desc));
        Expect(std::get<index_t>(imp0.desc) == 2);
        WasmImport& imp1 = test_module.imports[1];
        Expect(std::holds_alternative<index_t>(imp1.desc));
        Expect(std::get<index_t>(imp1.desc) == 0);
        WasmImport& imp2 = test_module.imports[2];
        Expect(std::holds_alternative<index_t>(imp2.desc));
        Expect(std::get<index_t>(imp2.desc) == 1);
        WasmImport& imp3 = test_module.imports[3];
        Expect(std::holds_alternative<index_t>(imp3.desc));
        Expect(std::get<index_t>(imp3.desc) == 3);
        WasmImport& imp4 = test_module.imports[4];
        Expect(std::holds_alternative<index_t>(imp4.desc));
        Expect(std::get<index_t>(imp4.desc) == 4);
    })
    Test("table", {

    })
    Test("memory", {

    })
    Test("global", {

    })
};