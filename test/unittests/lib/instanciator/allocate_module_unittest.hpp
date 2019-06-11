#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <Allocates.h>
#include <core/Store.h>
#include <dataTypes/Value.h>
#include <structures/WasmModule.h>
#include <structures/WasmTable.h>
#include <instance/ModuleInst.h>
#include <instance/TableInst.h>
}
#undef _Bool

SKYPAT_F(allocate_function, valid)
{
    // Prepare
    Store store = new_Store();
    char* moduleName = (char*) malloc(sizeof(char) * 5);
    strcpy(moduleName, "test");
    WasmModule* module = new_WasmModule(moduleName);

    FuncType type = new_FuncType();
    ValueType param1 = Value_i32;
    ValueType result1 = Value_i32;
    vector_push_back(type->params, &param1);
    vector_push_back(type->results, &result1);
    vector_push_back(module->types, type);

    TableInst* tableInst1 = new_TableInst();
    tableInst1->max = 20;
    vector_push_back(store->tables, tableInst1);
    TableInst* tableInst2 = new_TableInst();
    tableInst2->max = 20;
    vector_push_back(store->tables, tableInst2);

    ExportInst* exportInsts[1];
    exportInsts[0] = new_ExportInst();
    exportInsts[0]->name = NULL;
    exportInsts[0]->descType = Desc_Table;
    exportInsts[0]->valueAddr = 0;

    WasmTable table;
    table.min = 5;
    table.max = 30;
    vector_push_back(module->tables, &table);

    WasmExport export1;
    export1.name = (char*) malloc(sizeof(char) * 6);
    strcpy(export1.name, "table");
    export1.descType = Desc_Table;
    export1.descIdx = 1;
    vector_push_back(module->exports, &export1);

    // Test
    ModuleInst* moduleInst = allocate_Module(module, store, exportInsts, 1);
    EXPECT_FALSE(strcmp(moduleInst->name, module->module_name));
    EXPECT_EQ(vector_size(moduleInst->types), vector_size(module->types));
    EXPECT_EQ(vector_size(moduleInst->tableaddrs), 2);
    TableInst* internalTable = vector_at(TableInst*, store->tables, 2);
    EXPECT_EQ(vector_size(internalTable->elem), 5);
    EXPECT_EQ(internalTable->max, 30);
    ExportInst* internalExport = vector_at(ExportInst*, moduleInst->exports, 0);
    EXPECT_FALSE(strcmp(internalExport->name, "table"));
    EXPECT_EQ(internalExport->descType, Desc_Table);
    EXPECT_EQ(internalExport->valueAddr, 2);

    // Clean
    free_Store(store);
    free_WasmModule(module);
    free_ModuleInst(moduleInst);
}