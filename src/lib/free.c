/**
 * Copyright 2021 Luis Hsu, Kevin Cyu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include <WasmVM.h>
#include <structures/WasmModule.h>
#include <dataTypes/vector_t.h>

static void free_FuncType(FuncType* type)
{
    free_func(type->params.data);
    vector_init(type->params);
    free_func(type->results.data);
    vector_init(type->results);
}

static void free_Import(WasmImport* import)
{
    free_func(import->module.data);
    vector_init(import->module);
    free_func(import->name.data);
    vector_init(import->name);
}

static void free_Export(WasmExport* export)
{
    free_func(export->name.data);
    vector_init(export->name);
}

void module_free(wasm_module modulePtr)
{
    if(modulePtr != NULL) {
        WasmModule* module = modulePtr;
        // types
        for(unsigned int i = 0; i < module->types.size; ++i) {
            free_FuncType(module->types.data + i);
        }
        free_vector(module->types);
        // imports
        for(unsigned int i = 0; i < module->imports.size; ++i) {
            free_Import(module->imports.data + i);
        }
        free_vector(module->imports);
        // TODO: funcs
        // tables
        free_vector(module->tables);
        // mems
        free_vector(module->mems);
        // globals
        free_vector(module->globals);
        // exports
        for(unsigned int i = 0; i < module->exports.size; ++i) {
            free_Export(module->exports.data + i);
        }
        free_vector(module->exports);
        // TODO: elems
        // TODO: datas
        free_func(modulePtr);
        modulePtr = NULL;
    }
}