/**
 * Copyright 2021 Luis Hsu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include <WasmVM.h>

u32_t table_alloc(wasm_store store, wasm_table tabletype, wasm_ref ref)
{
    // Allocate TableInst
    u32_t index = store->tables.size;
    store->tables.size += 1;
    vector_resize(store->tables, TableInst, store->tables.size);
    // Fill tableInst
    TableInst* tableInst = store->tables.data + index;
    tableInst->type = tabletype.refType;
    tableInst->max = tabletype.max;
    // Elem
    vector_init(tableInst->elem);
    if(tabletype.min > 0) {
        tableInst->elem.size = tabletype.min;
        vector_resize(tableInst->elem, Ref, tabletype.min);
        for(unsigned int i = 0; i < tableInst->elem.size; ++i) {
            tableInst->elem.data[i].type = ref.type;
            tableInst->elem.data[i].addr = ref.addr;
            tableInst->elem.data[i].isNull = ref.isNull;
        }
    }
    return index;
}

wasm_table table_type(wasm_store store, u32_t address){
    wasm_table result;
    TableInst* table = store->tables.data + address;
    result.min = table->elem.size;
    result.max = table->max;
    result.refType = table->type;
    return result;
}

wasm_ref table_read(wasm_store store, u32_t address, u32_t index){
    if(index >= store->tables.data[address].elem.size){
        wasmvm_errno = ERROR_len_out_of_bound;
        wasm_ref ref = {.type = Ref_Unspecified, .isNull = 1, .addr = -1};
        return ref;
    }
    return store->tables.data[address].elem.data[index];
}

wasm_store table_write(wasm_store store, u32_t address, u32_t index, wasm_ref ref){
    if(index >= store->tables.data[address].elem.size){
        wasmvm_errno = ERROR_len_out_of_bound;
        return NULL;
    }
    store->tables.data[address].elem.data[index] = ref;
    return store;
}

u32_t table_size(wasm_store store, u32_t address){
    return store->tables.data[address].elem.size;
}

u32_t table_grow(wasm_store store, u32_t address, u32_t num, wasm_ref ref){
    TableInst* table = store->tables.data + address;
    u32_t old_size = table->elem.size;
    u32_t new_size = table->elem.size + num;
    if((new_size > table->max) || (new_size < old_size) || (new_size == -1)){
        return -1;
    }
    vector_resize(table->elem, wasm_ref, new_size);
    for(u32_t i = old_size; i < new_size; ++i){
        table->elem.data[i] = ref;
    }
}