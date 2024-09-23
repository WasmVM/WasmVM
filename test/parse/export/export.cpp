// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <harness.hpp>
#include <WasmVM.hpp>
#include <exception.hpp>

using namespace WasmVM;
using namespace Testing;

Suite exports {
    Test("exports", {
        ParseFile(test_module, "export.wat");
        WasmExport& exp0 = test_module.exports[4];
        Expect(exp0.name == "table0");
        Expect(exp0.desc == WasmExport::DescType::table);
        Expect(exp0.index == 0);
        WasmExport& exp1 = test_module.exports[5];
        Expect(exp1.name == "table1");
        Expect(exp1.desc == WasmExport::DescType::table);
        Expect(exp1.index == 0);
        WasmExport& exp2 = test_module.exports[2];
        Expect(exp2.name == "mem0");
        Expect(exp2.desc == WasmExport::DescType::mem);
        Expect(exp2.index == 0);
        WasmExport& exp3 = test_module.exports[3];
        Expect(exp3.name == "mem1");
        Expect(exp3.desc == WasmExport::DescType::mem);
        Expect(exp3.index == 0);
        WasmExport& exp4 = test_module.exports[0];
        Expect(exp4.name == "glo0");
        Expect(exp4.desc == WasmExport::DescType::global);
        Expect(exp4.index == 0);
        WasmExport& exp5 = test_module.exports[1];
        Expect(exp5.name == "glo1");
        Expect(exp5.desc == WasmExport::DescType::global);
        Expect(exp5.index == 0);
        // TODO: func
    })
};