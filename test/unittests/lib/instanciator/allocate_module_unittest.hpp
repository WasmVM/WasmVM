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
#include <instance/ModuleInst.h>
#include <instance/TableInst.h>
}
#undef _Bool

SKYPAT_F(allocate_function, valid)
{
    // Prepare
    Store* store = new_Store();
    char* moduleName = (char*) malloc(sizeof(char) * 5);
    strcpy(moduleName, "test");
    WasmModule* module = new_WasmModule(moduleName);

    FuncType* type = new_FuncType();
    ValueType param1 = Value_i32;
    ValueType result1 = Value_i32;
    type->params->push_back(type->params, &param1);
    type->results->push_back(type->results, &result1);
    module->types->push_back(module->types, type);

    TableInst* tableInst = new_TableInst();
    tableInst->max = 20;
    store->tables->push_back(store->tables, tableInst);

    ExportInst* exportInsts[1];
    exportInsts[0] = new_ExportInst();
    exportInsts[0]->name = NULL;
    exportInsts[0]->descType = Desc_Table;
    exportInsts[0]->valueAddr = 0;


    // Test
    ModuleInst* moduleInst = allocate_Module(module, store, exportInsts, 1);
    EXPECT_FALSE(strcmp(moduleInst->name, module->module_name));
    EXPECT_EQ(moduleInst->types->length, module->types->length);
    EXPECT_EQ(moduleInst->tableaddrs->length, 1);

    // Clean
    free_Store(store);
    free_WasmModule(module);
    free_ModuleInst(moduleInst);
}