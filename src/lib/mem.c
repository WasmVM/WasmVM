/**
 * Copyright 2021 Luis Hsu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include <WasmVM.h>

u32_t mem_alloc(wasm_store store, wasm_memory memtype)
{
    // Allocate MemInst
    u32_t index = store->mems.size;
    store->mems.size += 1;
    vector_resize(store->mems, MemInst, store->mems.size);
    // Fill memInst
    MemInst* memInst = store->mems.data + index;
    memInst->max = memtype.max;
    vector_init(memInst->data);
    memInst->data.size = 65536 * memtype.min;
    if(memInst->data.size > 0) {
        vector_resize(memInst->data, byte_t, memInst->data.size);
        memset_func(memInst->data.data, 0, sizeof(byte_t) * memInst->data.size);
    }
    return index;
}