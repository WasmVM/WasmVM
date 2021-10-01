/**
 * Copyright 2021 Luis Hsu, Kevin Cyu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include <WasmVM.h>
#include <structures/WasmModule.h>
#include <dataTypes/vector_t.h>

static void free_FuncType(FuncType type){
    free_func(type.params.data);
    free_func(type.results.data);
}

void module_free(wasm_module modulePtr)
{
    WasmModule* module = modulePtr;
    // types
    for(unsigned int i = 0; i < module->types.size; ++i){
        free_FuncType(module->types.data[i]);
    }
    free_vector(module->types);
}