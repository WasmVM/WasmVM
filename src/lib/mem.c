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

wasm_memory mem_type(wasm_store store, u32_t address){
    wasm_memory mem = {
        .min = store->mems.data[address].data.size,
        .max = store->mems.data[address].max
    };
    return mem;
}

byte_t mem_read(wasm_store store, u32_t address, u32_t index){
    if(index >= store->mems.data[address].data.size){
        wasmvm_errno = ERROR_len_out_of_bound;
        return 0;
    }
    return store->mems.data[address].data.data[index];
}

wasm_store mem_write(wasm_store store, u32_t address, u32_t index, byte_t data){
    if(index >= store->mems.data[address].data.size){
        wasmvm_errno = ERROR_len_out_of_bound;
        return NULL;
    }
    store->mems.data[address].data.data[index] = data;
    return store;
}

u32_t mem_size(wasm_store store, u32_t address){
    return store->mems.data[address].data.size;
}

wasm_store mem_grow(wasm_store store, u32_t address, u32_t npage){
    MemInst* mem = store->mems.data + address;
    u32_t old_size = mem->data.size / page_size;
    u32_t new_size = mem->data.size / page_size + npage;
    if((new_size > mem->max) || (new_size < old_size) || (new_size >= 65536)){
        wasmvm_errno = ERROR_len_out_of_bound;
        return NULL;
    }
    vector_resize(mem->data, byte_t, new_size * page_size);
    memset(mem->data.data + (old_size * page_size), 0, npage * page_size);
    return store;
}