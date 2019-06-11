#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <Instanciator_.h>
#include <structures/WasmModule.h>
#include <structures/WasmTable.h>
#include <structures/WasmElem.h>
#include <structures/WasmData.h>
#include <dataTypes/Value.h>
#include <dataTypes/vector_p.h>
#include <instance/ModuleInst.h>
#include <instance/FuncInst.h>
#include <instance/ExportInst.h>
#include <instance/GlobalInst.h>
#include <instance/MemInst.h>
#include <instance/TableInst.h>
}
#undef _Bool

SKYPAT_F(Instanciator, valid)
{
    // Prepare
    char* name1 = (char*) malloc(sizeof(char) * 6);
    strcpy(name1, "test1");
    WasmModule* module1 = new_WasmModule(name1);
    Executor executor = new_Executor();
    Instanciator instanciator = new_Instanciator(module1, executor);

    FuncType type1 = new_FuncType();
    vector_push_back(module1->types, type1);

    WasmFunc* func1 = new_WasmFunc();
    func1->type = 0;
    vector_push_back(module1->funcs, func1);
    WasmFunc* func2 = new_WasmFunc();
    func2->type = 0;
    vector_push_back(module1->funcs, func2);
    WasmFunc* func3 = new_WasmFunc();
    func3->type = 0;
    vector_push_back(module1->funcs, func3);

    char* importModule1 = (char*) malloc(sizeof(char)*7);
    strcpy(importModule1, "module");
    char* importModule2 = (char*) malloc(sizeof(char)*7);
    strcpy(importModule2, "module");
    char* importName1 = (char*) malloc(sizeof(char)*6);
    strcpy(importName1, "table");
    char* importName2 = (char*) malloc(sizeof(char)*6);
    strcpy(importName2, "table");

    WasmImport* tableImport = new_WasmImport(importModule1, importName1);
    tableImport->descType = Desc_Table;
    tableImport->desc.limits.min = 5;
    tableImport->desc.limits.max = 0;
    vector_push_back(module1->imports, tableImport);

    WasmElem* elem1 = new_WasmElem();
    elem1->table = 0;
    elem1->offset.value.i32 = 1;
    uint32_t elemAddr = 2;
    vector_push_back(elem1->init, &elemAddr);
    elemAddr = 1;
    vector_push_back(elem1->init, &elemAddr);
    vector_push_back(module1->elems, elem1);

    ModuleInst* importedModule = new_ModuleInst(importModule2);
    vector_push_back(executor_get_modules(executor), importedModule);

    uint32_t zero = 0;
    vector_push_back(importedModule->tableaddrs, &zero);
    TableInst* tableInst1 = new_TableInst();
    vector_resize(tableInst1->elem, 5);
    tableInst1->max = 0;
    vector_push_back(executor_get_store(executor)->tables, tableInst1);

    ExportInst* importedExport = new_ExportInst();
    importedExport->name = importName2;
    importedExport->descType = Desc_Table;
    importedExport->valueAddr = 0;
    vector_push_back(importedModule->exports, importedExport);

    WasmData* data1 = new_WasmData();
    data1->data = 0;
    data1->offset.value.i32 = 10;
    vector_resize(data1->init, 6);
    strcpy(vector_data(char*, data1->init), "Hello");
    vector_push_back(module1->datas, data1);

    WasmMemory memory;
    memory.min = 2;
    memory.max = 0;
    vector_push_back(module1->mems, &memory);

    module1->start = 0;

    // Test
    EXPECT_EQ(instanciator->parent.run((Stage*)instanciator), 0);
    EXPECT_EQ(vector_size(executor_get_modules(executor)), 2);
    EXPECT_EQ(*vector_at(uint32_t*, tableInst1->elem, 1), 2);
    EXPECT_EQ(*vector_at(uint32_t*, tableInst1->elem, 2), 1);
    MemInst* memInst = vector_at(MemInst*, executor_get_store(executor)->mems, 0);
    EXPECT_FALSE(strcmp(vector_at(char*, memInst->data, 10), "Hello"));

    // Clean
    free_Executor(executor);
    free(instanciator);
}

SKYPAT_F(Instanciator, no_imported_module)
{
    char* name = (char*) malloc(sizeof(char) * 5);
    strcpy(name, "test");
    WasmModule* module = new_WasmModule(name);
    Executor executor = new_Executor();
    Instanciator instanciator = new_Instanciator(module, executor);

    // Test
    char* importModule = (char*) malloc(sizeof(char)*7);
    strcpy(importModule, "module");
    char* importName = (char*) malloc(sizeof(char)*5);
    strcpy(importName, "func");
    WasmImport* import = new_WasmImport(importModule, importName);
    vector_push_back(module->imports, import);
    EXPECT_EQ(instanciator->parent.run((Stage*)instanciator), -1);

    // Clean
    free_Executor(executor);
    free(instanciator);
}

SKYPAT_F(Instanciator, function_not_match)
{
    // Prepare
    char* name = (char*) malloc(sizeof(char) * 5);
    strcpy(name, "test");
    WasmModule* module = new_WasmModule(name);
    vector_push_back(module->types, new_FuncType());
    Executor executor = new_Executor();
    Instanciator instanciator = new_Instanciator(module, executor);
    char* importModule1 = (char*) malloc(sizeof(char)*7);
    strcpy(importModule1, "module");

    ModuleInst* moduleInst = new_ModuleInst(importModule1);
    vector_push_back(executor_get_modules(executor), moduleInst);
    FuncType importedFuncType = new_FuncType();
    ValueType* param1 = (ValueType*)malloc(sizeof(ValueType));
    *param1 = Value_f32;
    vector_push_back(importedFuncType->params, param1);
    vector_push_back(moduleInst->types, importedFuncType);
    FuncInst* importedFunc = new_FuncInst(moduleInst, importedFuncType);
    vector_push_back(executor_get_store(executor)->funcs, importedFunc);

    ExportInst* exportInst = new_ExportInst();
    exportInst->name = (char*) malloc(sizeof(char) * 5);
    strcpy(exportInst->name, "func");
    exportInst->descType = Desc_Func;
    exportInst->valueAddr = 0;
    vector_push_back(moduleInst->exports, exportInst);

    // Test
    char* importModule2 = (char*) malloc(sizeof(char)*7);
    strcpy(importModule2, "module");
    char* importName = (char*) malloc(sizeof(char)*5);
    strcpy(importName, "func");
    WasmImport* import = new_WasmImport(importModule2, importName);
    import->descType = Desc_Func;
    import->desc.typeidx = 0;
    vector_push_back(module->imports, import);
    EXPECT_EQ(instanciator->parent.run((Stage*)instanciator), -2);

    // Clean
    free_Executor(executor);
    free(instanciator);
}

SKYPAT_F(Instanciator, global_mut_not_match)
{
    // Prepare
    char* name = (char*) malloc(sizeof(char) * 5);
    strcpy(name, "test");
    WasmModule* module = new_WasmModule(name);
    Executor executor = new_Executor();
    Instanciator instanciator = new_Instanciator(module, executor);
    char* importModule1 = (char*) malloc(sizeof(char)*7);
    strcpy(importModule1, "module");

    ModuleInst* moduleInst = new_ModuleInst(importModule1);
    vector_push_back(executor_get_modules(executor), moduleInst);
    uint32_t globalAddr = 0;
    vector_push_back(moduleInst->globaladdrs, &globalAddr);
    GlobalInst* importedGlobal = new_GlobalInst();
    importedGlobal->mut = 1;
    importedGlobal->value.type = Value_f32;
    vector_push_back(executor_get_store(executor)->globals, importedGlobal);

    ExportInst* exportInst = new_ExportInst();
    exportInst->name = (char*) malloc(sizeof(char) * 5);
    strcpy(exportInst->name, "global");
    exportInst->descType = Desc_Global;
    exportInst->valueAddr = 0;
    vector_push_back(moduleInst->exports, exportInst);

    // Test
    char* importModule2 = (char*) malloc(sizeof(char)*7);
    strcpy(importModule2, "module");
    char* importName = (char*) malloc(sizeof(char)*5);
    strcpy(importName, "global");
    WasmImport* import = new_WasmImport(importModule2, importName);
    import->descType = Desc_Global;
    import->desc.global.mut = 0;
    import->desc.global.valueType = Value_f32;
    vector_push_back(module->imports, import);
    EXPECT_EQ(instanciator->parent.run((Stage*)instanciator), -3);

    // Clean
    free_Executor(executor);
    free(instanciator);
}

SKYPAT_F(Instanciator, global_value_type_not_match)
{
    // Prepare
    char* name = (char*) malloc(sizeof(char) * 5);
    strcpy(name, "test");
    WasmModule* module = new_WasmModule(name);
    Executor executor = new_Executor();
    Instanciator instanciator = new_Instanciator(module, executor);
    char* importModule1 = (char*) malloc(sizeof(char)*7);
    strcpy(importModule1, "module");

    ModuleInst* moduleInst = new_ModuleInst(importModule1);
    vector_push_back(executor_get_modules(executor), moduleInst);
    uint32_t globalAddr = 0;
    vector_push_back(moduleInst->globaladdrs, &globalAddr);
    GlobalInst* importedGlobal = new_GlobalInst();
    importedGlobal->mut = 1;
    importedGlobal->value.type = Value_f32;
    vector_push_back(executor_get_store(executor)->globals, importedGlobal);

    ExportInst* exportInst = new_ExportInst();
    exportInst->name = (char*) malloc(sizeof(char) * 5);
    strcpy(exportInst->name, "global");
    exportInst->descType = Desc_Global;
    exportInst->valueAddr = 0;
    vector_push_back(moduleInst->exports, exportInst);

    // Test
    char* importModule2 = (char*) malloc(sizeof(char)*7);
    strcpy(importModule2, "module");
    char* importName = (char*) malloc(sizeof(char)*5);
    strcpy(importName, "global");
    WasmImport* import = new_WasmImport(importModule2, importName);
    import->descType = Desc_Global;
    import->desc.global.mut = 1;
    import->desc.global.valueType = Value_i32;
    vector_push_back(module->imports, import);
    EXPECT_EQ(instanciator->parent.run((Stage*)instanciator), -3);

    // Clean
    free_Executor(executor);
    free(instanciator);
}

SKYPAT_F(Instanciator, memory_min_not_match)
{
    // Prepare
    char* name = (char*) malloc(sizeof(char) * 5);
    strcpy(name, "test");
    WasmModule* module = new_WasmModule(name);
    Executor executor = new_Executor();
    Instanciator instanciator = new_Instanciator(module, executor);
    char* importModule1 = (char*) malloc(sizeof(char)*7);
    strcpy(importModule1, "module");

    ModuleInst* moduleInst = new_ModuleInst(importModule1);
    vector_push_back(executor_get_modules(executor), moduleInst);
    uint32_t memAddr = 0;
    vector_push_back(moduleInst->memaddrs, &memAddr);
    MemInst* importedMemory = new_MemInst();
    importedMemory->max = 2;
    vector_push_back(executor_get_store(executor)->mems, importedMemory);

    ExportInst* exportInst = new_ExportInst();
    exportInst->name = (char*) malloc(sizeof(char) * 7);
    strcpy(exportInst->name, "memory");
    exportInst->descType = Desc_Mem;
    exportInst->valueAddr = 0;
    vector_push_back(moduleInst->exports, exportInst);

    // Test
    char* importModule2 = (char*) malloc(sizeof(char)*7);
    strcpy(importModule2, "module");
    char* importName = (char*) malloc(sizeof(char)*7);
    strcpy(importName, "memory");
    WasmImport* import = new_WasmImport(importModule2, importName);
    import->descType = Desc_Mem;
    import->desc.limits.min = 1;
    import->desc.limits.max = 2;
    vector_push_back(module->imports, import);
    EXPECT_EQ(instanciator->parent.run((Stage*)instanciator), -4);

    // Clean
    free_Executor(executor);
    free(instanciator);
}

SKYPAT_F(Instanciator, memory_max_not_match)
{
    // Prepare
    char* name = (char*) malloc(sizeof(char) * 5);
    strcpy(name, "test");
    WasmModule* module = new_WasmModule(name);
    Executor executor = new_Executor();
    Instanciator instanciator = new_Instanciator(module, executor);
    char* importModule1 = (char*) malloc(sizeof(char)*7);
    strcpy(importModule1, "module");

    ModuleInst* moduleInst = new_ModuleInst(importModule1);
    vector_push_back(executor_get_modules(executor), moduleInst);
    uint32_t memAddr = 0;
    vector_push_back(moduleInst->memaddrs, &memAddr);
    MemInst* importedMemory = new_MemInst();
    importedMemory->max = 0;
    vector_push_back(executor_get_store(executor)->mems, importedMemory);

    ExportInst* exportInst = new_ExportInst();
    exportInst->name = (char*) malloc(sizeof(char) * 7);
    strcpy(exportInst->name, "memory");
    exportInst->descType = Desc_Mem;
    exportInst->valueAddr = 0;
    vector_push_back(moduleInst->exports, exportInst);

    // Test
    char* importModule2 = (char*) malloc(sizeof(char)*7);
    strcpy(importModule2, "module");
    char* importName = (char*) malloc(sizeof(char)*7);
    strcpy(importName, "memory");
    WasmImport* import = new_WasmImport(importModule2, importName);
    import->descType = Desc_Mem;
    import->desc.limits.min = 0;
    import->desc.limits.max = 2;
    vector_push_back(module->imports, import);
    EXPECT_EQ(instanciator->parent.run((Stage*)instanciator), -4);
    MemInst* memInstPtr = vector_at(MemInst*, executor_get_store(executor)->mems, 0);
    memInstPtr->max = 3;
    EXPECT_EQ(instanciator->parent.run((Stage*)instanciator), -4);

    // Clean
    free_Executor(executor);
    free(instanciator);
}

SKYPAT_F(Instanciator, table_min_not_match)
{
    // Prepare
    char* name = (char*) malloc(sizeof(char) * 5);
    strcpy(name, "test");
    WasmModule* module = new_WasmModule(name);
    Executor executor = new_Executor();
    Instanciator instanciator = new_Instanciator(module, executor);
    char* importModule1 = (char*) malloc(sizeof(char)*7);
    strcpy(importModule1, "module");

    ModuleInst* moduleInst = new_ModuleInst(importModule1);
    vector_push_back(executor_get_modules(executor), moduleInst);
    uint32_t tableAddr = 0;
    vector_push_back(moduleInst->tableaddrs, &tableAddr);
    TableInst* importedTable = new_TableInst();
    importedTable->max = 2;
    vector_push_back(executor_get_store(executor)->tables, importedTable);

    ExportInst* exportInst = new_ExportInst();
    exportInst->name = (char*) malloc(sizeof(char) * 6);
    strcpy(exportInst->name, "table");
    exportInst->descType = Desc_Table;
    exportInst->valueAddr = 0;
    vector_push_back(moduleInst->exports, exportInst);

    // Test
    char* importModule2 = (char*) malloc(sizeof(char) * 7);
    strcpy(importModule2, "module");
    char* importName = (char*) malloc(sizeof(char) * 6);
    strcpy(importName, "table");
    WasmImport* import = new_WasmImport(importModule2, importName);
    import->descType = Desc_Table;
    import->desc.limits.min = 1;
    import->desc.limits.max = 2;
    vector_push_back(module->imports, import);
    EXPECT_EQ(instanciator->parent.run((Stage*)instanciator), -5);

    // Clean
    free_Executor(executor);
    free(instanciator);
}

SKYPAT_F(Instanciator, table_max_not_match)
{
    // Prepare
    char* name = (char*) malloc(sizeof(char) * 5);
    strcpy(name, "test");
    WasmModule* module = new_WasmModule(name);
    Executor executor = new_Executor();
    Instanciator instanciator = new_Instanciator(module, executor);
    char* importModule1 = (char*) malloc(sizeof(char)*7);
    strcpy(importModule1, "module");

    ModuleInst* moduleInst = new_ModuleInst(importModule1);
    vector_push_back(executor_get_modules(executor), moduleInst);
    uint32_t tableAddr = 0;
    vector_push_back(moduleInst->tableaddrs, &tableAddr);
    TableInst* importedTable = new_TableInst();
    importedTable->max = 0;
    vector_push_back(executor_get_store(executor)->tables, importedTable);

    ExportInst* exportInst = new_ExportInst();
    exportInst->name = (char*) malloc(sizeof(char) * 6);
    strcpy(exportInst->name, "table");
    exportInst->descType = Desc_Table;
    exportInst->valueAddr = 0;
    vector_push_back(moduleInst->exports, exportInst);

    // Test
    char* importModule2 = (char*) malloc(sizeof(char)*7);
    strcpy(importModule2, "module");
    char* importName = (char*) malloc(sizeof(char)*6);
    strcpy(importName, "table");
    WasmImport* import = new_WasmImport(importModule2, importName);
    import->descType = Desc_Table;
    import->desc.limits.min = 0;
    import->desc.limits.max = 2;
    vector_push_back(module->imports, import);
    EXPECT_EQ(instanciator->parent.run((Stage*)instanciator), -5);
    TableInst* tableInstPtr = vector_at(TableInst*, executor_get_store(executor)->tables, 0);
    tableInstPtr->max = 3;
    EXPECT_EQ(instanciator->parent.run((Stage*)instanciator), -5);

    // Clean
    free_Executor(executor);
    free(instanciator);
}

SKYPAT_F(Instanciator, no_imported_name)
{
    // Prepare
    char* name = (char*) malloc(sizeof(char) * 5);
    strcpy(name, "test");
    WasmModule* module = new_WasmModule(name);
    Executor executor = new_Executor();
    Instanciator instanciator = new_Instanciator(module, executor);

    // Test
    char* importModule1 = (char*) malloc(sizeof(char)*7);
    strcpy(importModule1, "module");
    char* importModule2 = (char*) malloc(sizeof(char)*7);
    strcpy(importModule2, "module");
    ModuleInst* imported = new_ModuleInst(importModule1);
    vector_push_back(executor_get_modules(executor), imported);
    char* importName = (char*) malloc(sizeof(char)*5);
    strcpy(importName, "func");
    WasmImport* import = new_WasmImport(importModule2, importName);
    vector_push_back(module->imports, import);
    EXPECT_EQ(instanciator->parent.run((Stage*)instanciator), -7);

    // Clean
    free_Executor(executor);
    free(instanciator);
}

SKYPAT_F(Instanciator, elem_init_out_of_range)
{
    // Prepare
    char* name = (char*) malloc(sizeof(char) * 5);
    strcpy(name, "test");
    WasmModule* module = new_WasmModule(name);
    Executor executor = new_Executor();
    Instanciator instanciator = new_Instanciator(module, executor);

    WasmTable table;
    table.min = 3;
    table.max = 0;
    vector_push_back(module->tables, &table);

    WasmElem* elem = new_WasmElem();
    elem->table = 0;
    elem->offset.value.i32 = 2;
    elem->offset.type = Value_i32;
    uint32_t address = 2;
    vector_push_back(elem->init, &address);
    vector_push_back(elem->init, &address);
    vector_push_back(module->elems, elem);

    // Test
    EXPECT_EQ(instanciator->parent.run((Stage*)instanciator), -8);

    // Clean
    free_Executor(executor);
    free(instanciator);
}

SKYPAT_F(Instanciator, data_init_out_of_range)
{
    // Prepare
    char* name = (char*) malloc(sizeof(char) * 5);
    strcpy(name, "test");
    WasmModule* module = new_WasmModule(name);
    Executor executor = new_Executor();
    Instanciator instanciator = new_Instanciator(module, executor);

    WasmMemory memory;
    memory.min = 5;
    memory.max = 0;
    vector_push_back(module->mems, &memory);

    WasmData* data = new_WasmData();
    data->data = 0;
    data->offset.value.i32 = 65535 * 5;
    data->offset.type = Value_i32;
    vector_resize(data->init, sizeof(char) * 9);
    strcpy(vector_data(char*, data->init), "TestData");
    vector_push_back(module->datas, data);

    // Test
    EXPECT_EQ(instanciator->parent.run((Stage*)instanciator), -9);

    // Clean
    free_Executor(executor);
    free(instanciator);
}