// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <harness.hpp>
#include <WasmVM.hpp>
#include <exception.hpp>

using namespace WasmVM;
using namespace Testing;

Suite memory {
    Test("normal", {
        ParseFile(test_module, "normal.wat");
        MemType& mem0 = test_module.mems[0];
        Expect(mem0.min == 6);
        Expect(mem0.max.has_value() == false);
        MemType& mem1 = test_module.mems[1];
        Expect(mem1.min == 6);
        Expect(mem1.max.value() == 10);
        MemType& mem2 = test_module.mems[2];
        Expect(mem2.min == 7);
        Expect(mem2.max.has_value() == false);
    })
    Test("with_data", {
        // TODO:
    })
    Test("with_import", {
        ParseFile(test_module, "with_import.wat");

        WasmImport& imp0 = test_module.imports[0];
        Expect(std::holds_alternative<MemType>(imp0.desc));
        MemType& mem0 = std::get<MemType>(imp0.desc);
        Expect(imp0.module == "testmod");
        Expect(imp0.name == "mem0");
        Expect(mem0.min == 12);
        Expect(mem0.max.has_value() == false);

        WasmImport& imp1 = test_module.imports[1];
        Expect(std::holds_alternative<MemType>(imp1.desc));
        MemType& mem1 = std::get<MemType>(imp1.desc);
        Expect(imp1.module == "testmod");
        Expect(imp1.name == "mem1");
        Expect(mem1.min == 13);
        Expect(mem1.max.has_value() == false);

        WasmImport& imp2 = test_module.imports[2];
        Expect(std::holds_alternative<MemType>(imp2.desc));
        MemType& mem2 = std::get<MemType>(imp2.desc);
        Expect(imp2.module == "testmod");
        Expect(imp2.name == "mem3");
        Expect(mem2.min == 15);
        Expect(mem2.max.has_value() == false);

        MemType& mem3 = test_module.mems[0];
        Expect(mem3.min == 14);
        Expect(mem3.max.has_value() == false);
    })
    Test("with_export", {
        ParseFile(test_module, "with_export.wat");

        WasmImport& imp0 = test_module.imports[0];
        Expect(std::holds_alternative<MemType>(imp0.desc));
        MemType& memA = std::get<MemType>(imp0.desc);
        Expect(imp0.module == "testmod");
        Expect(imp0.name == "memA");
        Expect(memA.min == 6);
        Expect(memA.max.has_value() == false);

        MemType& mem0 = test_module.mems[0];
        Expect(mem0.min == 6);
        Expect(mem0.max.value() == 10);

        MemType& mem1 = test_module.mems[1];
        Expect(mem1.min == 8);
        Expect(mem1.max.value() == 12);

        MemType& mem2 = test_module.mems[2];
        Expect(mem2.min == 7);
        Expect(mem2.max.has_value() == false);

        WasmExport& export0 = test_module.exports[0];
        Expect(export0.name == "mem0");
        Expect(export0.desc == WasmExport::DescType::mem);
        Expect(export0.index == 0);

        WasmExport& export1 = test_module.exports[1];
        Expect(export1.name == "mem1");
        Expect(export1.desc == WasmExport::DescType::mem);
        Expect(export1.index == 1);

        WasmExport& export2 = test_module.exports[2];
        Expect(export2.name == "mem2");
        Expect(export2.desc == WasmExport::DescType::mem);
        Expect(export2.index == 2);

        WasmExport& export3 = test_module.exports[3];
        Expect(export3.name == "mem3");
        Expect(export3.desc == WasmExport::DescType::mem);
        Expect(export3.index == 2);

        WasmExport& export4 = test_module.exports[4];
        Expect(export4.name == "mem4");
        Expect(export4.desc == WasmExport::DescType::mem);
        Expect(export4.index == 3);
    })
};