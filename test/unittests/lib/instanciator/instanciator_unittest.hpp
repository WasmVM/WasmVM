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
    EXPECT_EQ(instanciator->parent.run((Stage*)instanciator), -2);

    // Clean
    free_Store(store);
    free_WasmModule(module);
    free_vector(moduleInsts);
    free(instanciator);
}