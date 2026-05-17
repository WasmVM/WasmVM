// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <harness.hpp>
#include <WasmVM.hpp>
#include <exception.hpp>

using namespace WasmVM;
using namespace Testing;

Suite func {
    Test("normal", {
        ParseFile(test_module, "normal.wat");
        WasmFunc& func0 = test_module.funcs[0];
        Expect(func0.typeidx == 0);
        Expect(func0.locals.size() == 0);
        Expect(func0.body.size() == 1);
        WasmFunc& func1 = test_module.funcs[1];
        Expect(func1.typeidx == 0);
        Expect(func1.locals.size() == 0);
        Expect(func1.body.size() == 1);
        WasmFunc& func2 = test_module.funcs[2];
        Expect(func2.typeidx == 0);
        Expect(func2.locals.size() == 1);
        Expect(func2.locals[0] == ValueType::i32);
        Expect(func2.body.size() == 1);
        WasmFunc& func3 = test_module.funcs[3];
        Expect(func3.typeidx == 0);
        Expect(func3.locals.size() == 2);
        Expect(func3.locals[0] == ValueType::i32);
        Expect(func3.locals[1] == ValueType::i64);
        Expect(func3.body.size() == 1);
        WasmFunc& func4 = test_module.funcs[4];
        Expect(func4.typeidx == 0);
        Expect(func4.locals.size() == 2);
        Expect(func4.locals[0] == ValueType::f32);
        Expect(func4.locals[1] == ValueType::i64);
        Expect(func4.body.size() == 1);
        WasmFunc& func5 = test_module.funcs[5];
        Expect(func5.typeidx == 0);
        Expect(func5.locals.size() == 1);
        Expect(func5.locals[0] == ValueType::i32);
        Expect(func5.body.size() == 1);
    })
    Test("with_import", {
        ParseFile(test_module, "with_import.wat");
        WasmImport& imp0 = test_module.imports[0];
        Expect(std::holds_alternative<index_t>(imp0.desc));
        index_t& typeidx0 = std::get<index_t>(imp0.desc);
        Expect(imp0.module == "testmod");
        Expect(imp0.name == "func0");
        Expect(typeidx0 == 0);

        WasmImport& imp1 = test_module.imports[1];
        Expect(std::holds_alternative<index_t>(imp1.desc));
        index_t& typeidx1 = std::get<index_t>(imp1.desc);
        Expect(imp1.module == "testmod");
        Expect(imp1.name == "func1");
        Expect(typeidx1 == 0);

        WasmImport& imp2 = test_module.imports[2];
        Expect(std::holds_alternative<index_t>(imp2.desc));
        index_t& typeidx2 = std::get<index_t>(imp2.desc);
        Expect(imp2.module == "testmod");
        Expect(imp2.name == "func3");
        Expect(typeidx2 == 0);

    })
    Test("with_export", {
        ParseFile(test_module, "with_export.wat");

        WasmImport& imp0 = test_module.imports[0];
        Expect(std::holds_alternative<index_t>(imp0.desc));
        index_t& typeidxA = std::get<index_t>(imp0.desc);
        Expect(imp0.module == "testmod");
        Expect(imp0.name == "funcA");
        Expect(typeidxA == 0);

        WasmFunc& func0 = test_module.funcs[0];
        Expect(func0.typeidx == 0);
        Expect(func0.locals.size() == 1);
        Expect(func0.locals[0] == ValueType::i32);
        Expect(func0.body.size() == 1);

        WasmFunc& func1 = test_module.funcs[1];
        Expect(func1.typeidx == 0);
        Expect(func1.locals.size() == 1);
        Expect(func1.locals[0] == ValueType::i64);
        Expect(func1.body.size() == 1);

        WasmFunc& func2 = test_module.funcs[2];
        Expect(func2.typeidx == 0);
        Expect(func2.locals.size() == 1);
        Expect(func2.locals[0] == ValueType::f32);
        Expect(func2.body.size() == 1);

        WasmExport& export0 = test_module.exports[0];
        Expect(export0.name == "func0");
        Expect(export0.desc == WasmExport::DescType::func);
        Expect(export0.index == 0);

        WasmExport& export1 = test_module.exports[1];
        Expect(export1.name == "func1");
        Expect(export1.desc == WasmExport::DescType::func);
        Expect(export1.index == 1);

        WasmExport& export2 = test_module.exports[2];
        Expect(export2.name == "func2");
        Expect(export2.desc == WasmExport::DescType::func);
        Expect(export2.index == 2);

        WasmExport& export3 = test_module.exports[3];
        Expect(export3.name == "func3");
        Expect(export3.desc == WasmExport::DescType::func);
        Expect(export3.index == 2);

        WasmExport& export4 = test_module.exports[4];
        Expect(export4.name == "func4");
        Expect(export4.desc == WasmExport::DescType::func);
        Expect(export4.index == 3);
    })
};