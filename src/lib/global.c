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
    globalInst->val.type = global.valType;
    globalInst->mut = global.mut;
    globalInst->val.value = val.value;
    return index;
}

wasm_global_type global_type(wasm_store store, u32_t address){
    return store->globals.data[address];
}

wasm_value global_read(wasm_store store, u32_t address){
    return store->globals.data[address].val;
}

wasm_store global_write(wasm_store store, u32_t address, wasm_value val){
    if(store->globals.data[address].mut){
        store->globals.data[address].val = val;
        return store;
    }else{
        wasmvm_errno = ERROR_immut_global;
        return NULL;
    }
}