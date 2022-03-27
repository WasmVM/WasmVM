/**
 * Copyright 2021 Luis Hsu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include <instances/ModuleInst.h>

wasm_module_inst module_inst_create()
{
    wasm_module_inst module = (wasm_module_inst)malloc_func(sizeof(ModuleInst));
    vector_init(module->types);
    vector_init(module->funcaddrs);
    vector_init(module->tableaddrs);
    vector_init(module->memaddrs);
    vector_init(module->globaladdrs);
    vector_init(module->elemaddrs);
    vector_init(module->dataaddrs);
    vector_init(module->exports);
    return module;
}
