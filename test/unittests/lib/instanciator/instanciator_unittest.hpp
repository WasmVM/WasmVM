#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <Instanciator.h>
#include <core/Store.h>
#include <structures/WasmModule.h>
#include <dataTypes/vector.h>
#include <instance/ModuleInst.h>
#include <instance/FuncInst.h>
#include <instance/ExportInst.h>
#include <instance/GlobalInst.h>
#include <instance/MemInst.h>
#include <instance/TableInst.h>
}
#undef _Bool

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
    free_WasmModule(module);
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
    free_WasmModule(module);
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
    free_WasmModule(module);
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
    free_WasmModule(module);
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
    free_WasmModule(module);
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
    free_WasmModule(module);
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
    free_WasmModule(module);
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
    free_WasmModule(module);
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
    free_WasmModule(module);
    free_vector(moduleInsts);
    free(instanciator);
}