#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <Instanciator.h>
#include <core/Store.h>
#include <structures/WasmModule.h>
#include <structures/WasmTable.h>
#include <structures/WasmElem.h>
#include <structures/WasmData.h>
#include <dataTypes/Value.h>
#include <dataTypes/vector.h>
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
    Store* store = new_Store();
    char* name1 = (char*) malloc(sizeof(char) * 6);
    strcpy(name1, "test1");
    WasmModule* module1 = new_WasmModule(name1);
    vector* moduleInsts = new_vector(sizeof(ModuleInst), (void(*)(void*))clean_ModuleInst);
    Instanciator* instanciator = new_Instanciator(module1, store, moduleInsts);

    FuncType* type1 = new_FuncType();
    module1->types->push_back(module1->types, type1);

    WasmFunc* func1 = new_WasmFunc();
    func1->type = 0;
    module1->funcs->push_back(module1->funcs, func1);
    WasmFunc* func2 = new_WasmFunc();
    func2->type = 0;
    module1->funcs->push_back(module1->funcs, func2);
    WasmFunc* func3 = new_WasmFunc();
    func3->type = 0;
    module1->funcs->push_back(module1->funcs, func3);

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
    module1->imports->push_back(module1->imports, tableImport);

    WasmElem* elem1 = new_WasmElem();
    elem1->table = 0;
    elem1->offset.value.i32 = 1;
    uint32_t elemAddr = 2;
    elem1->init->push_back(elem1->init, &elemAddr);
    elemAddr = 1;
    elem1->init->push_back(elem1->init, &elemAddr);
    module1->elems->push_back(module1->elems, elem1);

    ModuleInst* importedModule = new_ModuleInst(importModule2);
    moduleInsts->push_back(moduleInsts, importedModule);

    uint32_t zero = 0;
    importedModule->tableaddrs->push_back(importedModule->tableaddrs, &zero);
    TableInst* tableInst1 = new_TableInst();
    tableInst1->elem->resize(tableInst1->elem, 5);
    tableInst1->max = 0;
    store->tables->push_back(store->tables, tableInst1);

    ExportInst* importedExport = new_ExportInst();
    importedExport->name = importName2;
    importedExport->descType = Desc_Table;
    importedExport->valueAddr = 0;
    importedModule->exports->push_back(importedModule->exports, importedExport);

    WasmData* data1 = new_WasmData();
    data1->data = 0;
    data1->offset.value.i32 = 10;
    data1->init->resize(data1->init, 6);
    strcpy((char*)data1->init->data, "Hello");
    module1->datas->push_back(module1->datas, data1);

    WasmMemory memory;
    memory.min = 2;
    memory.max = 0;
    module1->mems->push_back(module1->mems, &memory);

    // Test
    EXPECT_EQ(instanciator->parent.run((Stage*)instanciator), 0);
    EXPECT_EQ(*(uint32_t*)tableInst1->elem->at(tableInst1->elem, 1), 2);
    EXPECT_EQ(*(uint32_t*)tableInst1->elem->at(tableInst1->elem, 2), 1);
    MemInst* memInst = (MemInst*)store->mems->at(store->mems, 0);
    EXPECT_FALSE(strcmp((char*)memInst->data->at(memInst->data, 10), "Hello"));

    // Clean
    free_Store(store);
    free_vector(moduleInsts);
    free(instanciator);
}

SKYPAT_F(Instanciator, no_imported_module)
{
    // Prepare
    Store* store = new_Store();
    char* name = (char*) malloc(sizeof(char) * 5);
    strcpy(name, "test");
    WasmModule* module = new_WasmModule(name);
    vector* moduleInsts = new_vector(sizeof(ModuleInst), (void(*)(void*))clean_ModuleInst);
    Instanciator* instanciator = new_Instanciator(module, store, moduleInsts);

    // Test
    char* importModule = (char*) malloc(sizeof(char)*7);
    strcpy(importModule, "module");
    char* importName = (char*) malloc(sizeof(char)*5);
    strcpy(importName, "func");
    WasmImport* import = new_WasmImport(importModule, importName);
    module->imports->push_back(module->imports, import);
    EXPECT_EQ(instanciator->parent.run((Stage*)instanciator), -1);

    // Clean
    free_Store(store);
    free_vector(moduleInsts);
    free(instanciator);
}

SKYPAT_F(Instanciator, function_not_match)
{
    // Prepare
    Store* store = new_Store();
    char* name = (char*) malloc(sizeof(char) * 5);
    strcpy(name, "test");
    WasmModule* module = new_WasmModule(name);
    module->types->push_back(module->types, new_FuncType());
    vector* moduleInsts = new_vector(sizeof(ModuleInst), (void(*)(void*))clean_ModuleInst);
    Instanciator* instanciator = new_Instanciator(module, store, moduleInsts);
    char* importModule1 = (char*) malloc(sizeof(char)*7);
    strcpy(importModule1, "module");

    ModuleInst* moduleInst = new_ModuleInst(importModule1);
    moduleInsts->push_back(moduleInsts, moduleInst);
    FuncType* importedFuncType = new_FuncType();
    ValueType* param1 = (ValueType*)malloc(sizeof(ValueType));
    *param1 = Value_f32;
    importedFuncType->params->push_back(importedFuncType->params, param1);
    moduleInst->types->push_back(moduleInst->types, importedFuncType);
    FuncInst* importedFunc = new_FuncInst(moduleInst, importedFuncType);
    store->funcs->push_back(store->funcs, importedFunc);

    ExportInst* exportInst = new_ExportInst();
    exportInst->name = (char*) malloc(sizeof(char) * 5);
    strcpy(exportInst->name, "func");
    exportInst->descType = Desc_Func;
    exportInst->valueAddr = 0;
    moduleInst->exports->push_back(moduleInst->exports, exportInst);

    // Test
    char* importModule2 = (char*) malloc(sizeof(char)*7);
    strcpy(importModule2, "module");
    char* importName = (char*) malloc(sizeof(char)*5);
    strcpy(importName, "func");
    WasmImport* import = new_WasmImport(importModule2, importName);
    import->descType = Desc_Func;
    import->desc.typeidx = 0;
    module->imports->push_back(module->imports, import);
    EXPECT_EQ(instanciator->parent.run((Stage*)instanciator), -2);

    // Clean
    free_Store(store);
    free_vector(moduleInsts);
    free(instanciator);
}

SKYPAT_F(Instanciator, global_mut_not_match)
{
    // Prepare
    Store* store = new_Store();
    char* name = (char*) malloc(sizeof(char) * 5);
    strcpy(name, "test");
    WasmModule* module = new_WasmModule(name);
    vector* moduleInsts = new_vector(sizeof(ModuleInst), (void(*)(void*))clean_ModuleInst);
    Instanciator* instanciator = new_Instanciator(module, store, moduleInsts);
    char* importModule1 = (char*) malloc(sizeof(char)*7);
    strcpy(importModule1, "module");

    ModuleInst* moduleInst = new_ModuleInst(importModule1);
    moduleInsts->push_back(moduleInsts, moduleInst);
    uint32_t globalAddr = 0;
    moduleInst->globaladdrs->push_back(moduleInst->globaladdrs, &globalAddr);
    GlobalInst* importedGlobal = new_GlobalInst();
    importedGlobal->mut = 1;
    importedGlobal->value.type = Value_f32;
    store->globals->push_back(store->globals, importedGlobal);

    ExportInst* exportInst = new_ExportInst();
    exportInst->name = (char*) malloc(sizeof(char) * 5);
    strcpy(exportInst->name, "global");
    exportInst->descType = Desc_Global;
    exportInst->valueAddr = 0;
    moduleInst->exports->push_back(moduleInst->exports, exportInst);

    // Test
    char* importModule2 = (char*) malloc(sizeof(char)*7);
    strcpy(importModule2, "module");
    char* importName = (char*) malloc(sizeof(char)*5);
    strcpy(importName, "global");
    WasmImport* import = new_WasmImport(importModule2, importName);
    import->descType = Desc_Global;
    import->desc.global.mut = 0;
    import->desc.global.valueType = Value_f32;
    module->imports->push_back(module->imports, import);
    EXPECT_EQ(instanciator->parent.run((Stage*)instanciator), -3);

    // Clean
    free_Store(store);
    free_vector(moduleInsts);
    free(instanciator);
}

SKYPAT_F(Instanciator, global_value_type_not_match)
{
    // Prepare
    Store* store = new_Store();
    char* name = (char*) malloc(sizeof(char) * 5);
    strcpy(name, "test");
    WasmModule* module = new_WasmModule(name);
    vector* moduleInsts = new_vector(sizeof(ModuleInst), (void(*)(void*))clean_ModuleInst);
    Instanciator* instanciator = new_Instanciator(module, store, moduleInsts);
    char* importModule1 = (char*) malloc(sizeof(char)*7);
    strcpy(importModule1, "module");

    ModuleInst* moduleInst = new_ModuleInst(importModule1);
    moduleInsts->push_back(moduleInsts, moduleInst);
    uint32_t globalAddr = 0;
    moduleInst->globaladdrs->push_back(moduleInst->globaladdrs, &globalAddr);
    GlobalInst* importedGlobal = new_GlobalInst();
    importedGlobal->mut = 1;
    importedGlobal->value.type = Value_f32;
    store->globals->push_back(store->globals, importedGlobal);

    ExportInst* exportInst = new_ExportInst();
    exportInst->name = (char*) malloc(sizeof(char) * 5);
    strcpy(exportInst->name, "global");
    exportInst->descType = Desc_Global;
    exportInst->valueAddr = 0;
    moduleInst->exports->push_back(moduleInst->exports, exportInst);

    // Test
    char* importModule2 = (char*) malloc(sizeof(char)*7);
    strcpy(importModule2, "module");
    char* importName = (char*) malloc(sizeof(char)*5);
    strcpy(importName, "global");
    WasmImport* import = new_WasmImport(importModule2, importName);
    import->descType = Desc_Global;
    import->desc.global.mut = 1;
    import->desc.global.valueType = Value_i32;
    module->imports->push_back(module->imports, import);
    EXPECT_EQ(instanciator->parent.run((Stage*)instanciator), -3);

    // Clean
    free_Store(store);
    free_vector(moduleInsts);
    free(instanciator);
}

SKYPAT_F(Instanciator, memory_min_not_match)
{
    // Prepare
    Store* store = new_Store();
    char* name = (char*) malloc(sizeof(char) * 5);
    strcpy(name, "test");
    WasmModule* module = new_WasmModule(name);
    vector* moduleInsts = new_vector(sizeof(ModuleInst), (void(*)(void*))clean_ModuleInst);
    Instanciator* instanciator = new_Instanciator(module, store, moduleInsts);
    char* importModule1 = (char*) malloc(sizeof(char)*7);
    strcpy(importModule1, "module");

    ModuleInst* moduleInst = new_ModuleInst(importModule1);
    moduleInsts->push_back(moduleInsts, moduleInst);
    uint32_t memAddr = 0;
    moduleInst->memaddrs->push_back(moduleInst->memaddrs, &memAddr);
    MemInst* importedMemory = new_MemInst();
    importedMemory->max = 2;
    store->mems->push_back(store->mems, importedMemory);

    ExportInst* exportInst = new_ExportInst();
    exportInst->name = (char*) malloc(sizeof(char) * 7);
    strcpy(exportInst->name, "memory");
    exportInst->descType = Desc_Mem;
    exportInst->valueAddr = 0;
    moduleInst->exports->push_back(moduleInst->exports, exportInst);

    // Test
    char* importModule2 = (char*) malloc(sizeof(char)*7);
    strcpy(importModule2, "module");
    char* importName = (char*) malloc(sizeof(char)*7);
    strcpy(importName, "memory");
    WasmImport* import = new_WasmImport(importModule2, importName);
    import->descType = Desc_Mem;
    import->desc.limits.min = 1;
    import->desc.limits.max = 2;
    module->imports->push_back(module->imports, import);
    EXPECT_EQ(instanciator->parent.run((Stage*)instanciator), -4);

    // Clean
    free_Store(store);
    free_vector(moduleInsts);
    free(instanciator);
}

SKYPAT_F(Instanciator, memory_max_not_match)
{
    // Prepare
    Store* store = new_Store();
    char* name = (char*) malloc(sizeof(char) * 5);
    strcpy(name, "test");
    WasmModule* module = new_WasmModule(name);
    vector* moduleInsts = new_vector(sizeof(ModuleInst), (void(*)(void*))clean_ModuleInst);
    Instanciator* instanciator = new_Instanciator(module, store, moduleInsts);
    char* importModule1 = (char*) malloc(sizeof(char)*7);
    strcpy(importModule1, "module");

    ModuleInst* moduleInst = new_ModuleInst(importModule1);
    moduleInsts->push_back(moduleInsts, moduleInst);
    uint32_t memAddr = 0;
    moduleInst->memaddrs->push_back(moduleInst->memaddrs, &memAddr);
    MemInst* importedMemory = new_MemInst();
    importedMemory->max = 0;
    store->mems->push_back(store->mems, importedMemory);

    ExportInst* exportInst = new_ExportInst();
    exportInst->name = (char*) malloc(sizeof(char) * 7);
    strcpy(exportInst->name, "memory");
    exportInst->descType = Desc_Mem;
    exportInst->valueAddr = 0;
    moduleInst->exports->push_back(moduleInst->exports, exportInst);

    // Test
    char* importModule2 = (char*) malloc(sizeof(char)*7);
    strcpy(importModule2, "module");
    char* importName = (char*) malloc(sizeof(char)*7);
    strcpy(importName, "memory");
    WasmImport* import = new_WasmImport(importModule2, importName);
    import->descType = Desc_Mem;
    import->desc.limits.min = 0;
    import->desc.limits.max = 2;
    module->imports->push_back(module->imports, import);
    EXPECT_EQ(instanciator->parent.run((Stage*)instanciator), -4);
    MemInst* memInstPtr = (MemInst*)store->mems->at(store->mems, 0);
    memInstPtr->max = 3;
    EXPECT_EQ(instanciator->parent.run((Stage*)instanciator), -4);

    // Clean
    free_Store(store);
    free_vector(moduleInsts);
    free(instanciator);
}

SKYPAT_F(Instanciator, table_min_not_match)
{
    // Prepare
    Store* store = new_Store();
    char* name = (char*) malloc(sizeof(char) * 5);
    strcpy(name, "test");
    WasmModule* module = new_WasmModule(name);
    vector* moduleInsts = new_vector(sizeof(ModuleInst), (void(*)(void*))clean_ModuleInst);
    Instanciator* instanciator = new_Instanciator(module, store, moduleInsts);
    char* importModule1 = (char*) malloc(sizeof(char)*7);
    strcpy(importModule1, "module");

    ModuleInst* moduleInst = new_ModuleInst(importModule1);
    moduleInsts->push_back(moduleInsts, moduleInst);
    uint32_t tableAddr = 0;
    moduleInst->tableaddrs->push_back(moduleInst->tableaddrs, &tableAddr);
    TableInst* importedTable = new_TableInst();
    importedTable->max = 2;
    store->tables->push_back(store->tables, importedTable);

    ExportInst* exportInst = new_ExportInst();
    exportInst->name = (char*) malloc(sizeof(char) * 6);
    strcpy(exportInst->name, "table");
    exportInst->descType = Desc_Table;
    exportInst->valueAddr = 0;
    moduleInst->exports->push_back(moduleInst->exports, exportInst);

    // Test
    char* importModule2 = (char*) malloc(sizeof(char) * 7);
    strcpy(importModule2, "module");
    char* importName = (char*) malloc(sizeof(char) * 6);
    strcpy(importName, "table");
    WasmImport* import = new_WasmImport(importModule2, importName);
    import->descType = Desc_Table;
    import->desc.limits.min = 1;
    import->desc.limits.max = 2;
    module->imports->push_back(module->imports, import);
    EXPECT_EQ(instanciator->parent.run((Stage*)instanciator), -5);

    // Clean
    free_Store(store);
    free_vector(moduleInsts);
    free(instanciator);
}

SKYPAT_F(Instanciator, table_max_not_match)
{
    // Prepare
    Store* store = new_Store();
    char* name = (char*) malloc(sizeof(char) * 5);
    strcpy(name, "test");
    WasmModule* module = new_WasmModule(name);
    vector* moduleInsts = new_vector(sizeof(ModuleInst), (void(*)(void*))clean_ModuleInst);
    Instanciator* instanciator = new_Instanciator(module, store, moduleInsts);
    char* importModule1 = (char*) malloc(sizeof(char)*7);
    strcpy(importModule1, "module");

    ModuleInst* moduleInst = new_ModuleInst(importModule1);
    moduleInsts->push_back(moduleInsts, moduleInst);
    uint32_t tableAddr = 0;
    moduleInst->tableaddrs->push_back(moduleInst->tableaddrs, &tableAddr);
    TableInst* importedTable = new_TableInst();
    importedTable->max = 0;
    store->tables->push_back(store->tables, importedTable);

    ExportInst* exportInst = new_ExportInst();
    exportInst->name = (char*) malloc(sizeof(char) * 6);
    strcpy(exportInst->name, "table");
    exportInst->descType = Desc_Table;
    exportInst->valueAddr = 0;
    moduleInst->exports->push_back(moduleInst->exports, exportInst);

    // Test
    char* importModule2 = (char*) malloc(sizeof(char)*7);
    strcpy(importModule2, "module");
    char* importName = (char*) malloc(sizeof(char)*6);
    strcpy(importName, "table");
    WasmImport* import = new_WasmImport(importModule2, importName);
    import->descType = Desc_Table;
    import->desc.limits.min = 0;
    import->desc.limits.max = 2;
    module->imports->push_back(module->imports, import);
    EXPECT_EQ(instanciator->parent.run((Stage*)instanciator), -5);
    TableInst* tableInstPtr = (TableInst*)store->tables->at(store->tables, 0);
    tableInstPtr->max = 3;
    EXPECT_EQ(instanciator->parent.run((Stage*)instanciator), -5);

    // Clean
    free_Store(store);
    free_vector(moduleInsts);
    free(instanciator);
}

SKYPAT_F(Instanciator, no_imported_name)
{
    // Prepare
    Store* store = new_Store();
    char* name = (char*) malloc(sizeof(char) * 5);
    strcpy(name, "test");
    WasmModule* module = new_WasmModule(name);
    vector* moduleInsts = new_vector(sizeof(ModuleInst), (void(*)(void*))clean_ModuleInst);
    Instanciator* instanciator = new_Instanciator(module, store, moduleInsts);

    // Test
    char* importModule1 = (char*) malloc(sizeof(char)*7);
    strcpy(importModule1, "module");
    char* importModule2 = (char*) malloc(sizeof(char)*7);
    strcpy(importModule2, "module");
    ModuleInst* imported = new_ModuleInst(importModule1);
    moduleInsts->push_back(moduleInsts, imported);
    char* importName = (char*) malloc(sizeof(char)*5);
    strcpy(importName, "func");
    WasmImport* import = new_WasmImport(importModule2, importName);
    module->imports->push_back(module->imports, import);
    EXPECT_EQ(instanciator->parent.run((Stage*)instanciator), -7);

    // Clean
    free_Store(store);
    free_vector(moduleInsts);
    free(instanciator);
}

SKYPAT_F(Instanciator, elem_init_out_of_range)
{
    // Prepare
    Store* store = new_Store();
    char* name = (char*) malloc(sizeof(char) * 5);
    strcpy(name, "test");
    WasmModule* module = new_WasmModule(name);
    vector* moduleInsts = new_vector(sizeof(ModuleInst), (void(*)(void*))clean_ModuleInst);
    Instanciator* instanciator = new_Instanciator(module, store, moduleInsts);

    WasmTable table;
    table.min = 3;
    table.max = 0;
    module->tables->push_back(module->tables, &table);

    WasmElem* elem = new_WasmElem();
    elem->table = 0;
    elem->offset.value.i32 = 2;
    elem->offset.type = Value_i32;
    uint32_t address = 2;
    elem->init->push_back(elem->init, &address);
    elem->init->push_back(elem->init, &address);
    module->elems->push_back(module->elems, elem);

    // Test
    EXPECT_EQ(instanciator->parent.run((Stage*)instanciator), -8);

    // Clean
    free_Store(store);
    free_vector(moduleInsts);
    free(instanciator);
}

SKYPAT_F(Instanciator, data_init_out_of_range)
{
    // Prepare
    Store* store = new_Store();
    char* name = (char*) malloc(sizeof(char) * 5);
    strcpy(name, "test");
    WasmModule* module = new_WasmModule(name);
    vector* moduleInsts = new_vector(sizeof(ModuleInst), (void(*)(void*))clean_ModuleInst);
    Instanciator* instanciator = new_Instanciator(module, store, moduleInsts);

    WasmMemory memory;
    memory.min = 5;
    memory.max = 0;
    module->mems->push_back(module->mems, &memory);

    WasmData* data = new_WasmData();
    data->data = 0;
    data->offset.value.i32 = 65535 * 5;
    data->offset.type = Value_i32;
    data->init->resize(data->init, sizeof(char) * 9);
    strcpy((char*)data->init->data, "TestData");
    module->datas->push_back(module->datas, data);

    // Test
    EXPECT_EQ(instanciator->parent.run((Stage*)instanciator), -9);

    // Clean
    free_Store(store);
    free_vector(moduleInsts);
    free(instanciator);
}