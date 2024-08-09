// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <harness.hpp>
#include <WasmVM.hpp>
#include <exception.hpp>

using namespace WasmVM;
using namespace Testing;

Suite table {
    Test("normal", {
        ParseFile(test_module, "normal.wat");
        TableType& table0 = test_module.tables[0];
        Expect(table0.limits.min == 6);
        Expect(table0.limits.max.has_value() == false);
        Expect(table0.reftype == RefType::funcref);
        TableType& table1 = test_module.tables[1];
        Expect(table1.limits.min == 6);
        Expect(table1.limits.max.value() == 10);
        Expect(table1.reftype == RefType::funcref);
        TableType& table2 = test_module.tables[2];
        Expect(table2.limits.min == 8);
        Expect(table2.limits.max.value() == 20);
        Expect(table2.reftype == RefType::externref);
        TableType& table3 = test_module.tables[3];
        Expect(table3.limits.min == 7);
        Expect(table0.limits.max.has_value() == false);
        Expect(table3.reftype == RefType::funcref);
    })
    Test("with_elem", {
        // TODO:
    })
    Test("with_import", {
        ParseFile(test_module, "with_import.wat");

        WasmImport& imp0 = test_module.imports[0];
        Expect(std::holds_alternative<TableType>(imp0.desc));
        TableType& table0 = std::get<TableType>(imp0.desc);
        Expect(imp0.module == "testmod");
        Expect(imp0.name == "table0");
        Expect(table0.limits.min == 12);
        Expect(table0.limits.max.has_value() == false);
        Expect(table0.reftype == RefType::funcref);

        WasmImport& imp1 = test_module.imports[1];
        Expect(std::holds_alternative<TableType>(imp1.desc));
        TableType& table1 = std::get<TableType>(imp1.desc);
        Expect(imp1.module == "testmod");
        Expect(imp1.name == "table1");
        Expect(table1.limits.min == 13);
        Expect(table1.limits.max.has_value() == false);
        Expect(table1.reftype == RefType::funcref);

        WasmImport& imp2 = test_module.imports[2];
        Expect(std::holds_alternative<TableType>(imp2.desc));
        TableType& table2 = std::get<TableType>(imp2.desc);
        Expect(imp2.module == "testmod");
        Expect(imp2.name == "table3");
        Expect(table2.limits.min == 15);
        Expect(table2.limits.max.has_value() == false);
        Expect(table2.reftype == RefType::funcref);

        TableType& table3 = test_module.tables[0];
        Expect(table3.limits.min == 14);
        Expect(table3.limits.max.has_value() == false);
        Expect(table3.reftype == RefType::externref);
    })
    Test("with_export", {
        ParseFile(test_module, "with_export.wat");

        WasmImport& imp0 = test_module.imports[0];
        Expect(std::holds_alternative<TableType>(imp0.desc));
        TableType& tableA = std::get<TableType>(imp0.desc);
        Expect(imp0.module == "testmod");
        Expect(imp0.name == "tableA");
        Expect(tableA.limits.min == 6);
        Expect(tableA.limits.max.value() == 10);
        Expect(tableA.reftype == RefType::funcref);

        TableType& table0 = test_module.tables[0];
        Expect(table0.limits.min == 6);
        Expect(table0.limits.max.has_value() == false);
        Expect(table0.reftype == RefType::funcref);

        TableType& table1 = test_module.tables[1];
        Expect(table1.limits.min == 8);
        Expect(table1.limits.max.value() == 20);
        Expect(table1.reftype == RefType::externref);

        TableType& table2 = test_module.tables[2];
        Expect(table2.limits.min == 7);
        Expect(table2.limits.max.has_value() == false);
        Expect(table2.reftype == RefType::funcref);

        WasmExport& export0 = test_module.exports[0];
        Expect(export0.name == "table0");
        Expect(export0.desc == WasmExport::DescType::table);
        Expect(export0.index == 0);

        WasmExport& export1 = test_module.exports[1];
        Expect(export1.name == "table1");
        Expect(export1.desc == WasmExport::DescType::table);
        Expect(export1.index == 1);

        WasmExport& export2 = test_module.exports[2];
        Expect(export2.name == "table2");
        Expect(export2.desc == WasmExport::DescType::table);
        Expect(export2.index == 2);

        WasmExport& export3 = test_module.exports[3];
        Expect(export3.name == "table3");
        Expect(export3.desc == WasmExport::DescType::table);
        Expect(export3.index == 2);

        WasmExport& export4 = test_module.exports[4];
        Expect(export4.name == "table4");
        Expect(export4.desc == WasmExport::DescType::table);
        Expect(export4.index == 3);
    })
};