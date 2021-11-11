/**
 * Copyright 2021 Luis Hsu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include <WasmVM.h>

u32_t global_alloc(wasm_store store, wasm_global global, wasm_value val)
{
    // Allocate GlobalInst
    u32_t index = store->globals.size;
    store->globals.size += 1;
    vector_resize(store->globals, GlobalInst, store->globals.size);
    // Fill GlobalInst
    GlobalInst* globalInst = store->globals.data + index;
    globalInst->type = global.valType;
    globalInst->value = val.value;
    return index;
}