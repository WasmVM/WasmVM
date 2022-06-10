/**
 * Copyright 2021 Luis Hsu, Kevin Cyu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include <WasmVM.h>
#include <defines.h>

Store* store_init()
{
    Store* store = (Store*) malloc_func(sizeof(Store));
    vector_init(store->funcs);
    vector_init(store->tables);
    vector_init(store->mems);
    vector_init(store->globals);
    vector_init(store->elems);
    vector_init(store->datas);
    return store;
}

static void free_FuncInst(FuncInst func){
    if(func.bodyType == FuncBody_Wasm){
        free_vector(func.body.wasm.locals);
        free_vector(func.body.wasm.codes);
    }
}

void store_free(Store* store)
{
    if(store != NULL) {
        // Funcs
        for(u32_t i = 0; i < store->funcs.size; ++i){
            free_FuncInst(store->funcs.data[i]);
        }
        free_vector(store->funcs);
        // Tables
        for(u32_t i = 0; i < store->tables.size; ++i){
            free_vector(store->tables.data[i].elem);
        }
        free_vector(store->tables);
        // Mems
        for(u32_t i = 0; i < store->mems.size; ++i){
            free_vector(store->mems.data[i].data);
        }
        free_vector(store->mems);
        free_vector(store->globals);        // FIXME:
        free_vector(store->elems);        // FIXME:
        // Datas
        for(u32_t i = 0; i < store->datas.size; ++i){
            free_vector(store->datas.data[i].data);
        }
        free_vector(store->datas);
        // Store
        free_func(store);
    }
}