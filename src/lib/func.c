/**
 * Copyright 2021 Luis Hsu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include <WasmVM.h>

u32_t func_alloc(wasm_store store, wasm_functype* const functype, hostfunc_t hostfunc)
{
    // Allocate FuncInst
    u32_t index = store->funcs.size;
    store->funcs.size += 1;
    vector_resize(store->funcs, FuncInst, store->funcs.size);
    // Fill FuncInst
    FuncInst* funcInst = store->funcs.data + index;
    funcInst->bodyType = FuncBody_Host;
    vector_init(funcInst->type.params);
    if(functype->params.size > 0){
        funcInst->type.params.size = functype->params.size;
        vector_resize(funcInst->type.params, ValueType, sizeof(ValueType) * functype->params.size);
        memcpy_func(funcInst->type.params.data, functype->params.data, sizeof(ValueType) * functype->params.size);
    }
    vector_init(funcInst->type.results);
    if(functype->results.size > 0){
        funcInst->type.results.size = functype->results.size;
        vector_resize(funcInst->type.results, ValueType, sizeof(ValueType) * functype->results.size);
        memcpy_func(funcInst->type.results.data, functype->results.data, sizeof(ValueType) * functype->results.size);
    }
    funcInst->body.hostcode = hostfunc;
    return index;
}

_Bool check_FuncType(FuncType* type1, FuncType* type2){
    if((type1->params.size != type2->params.size) || (type1->results.size != type2->results.size)){
        return 0;
    }
    for(u32_t i = 0; i < type1->params.size; ++i){
        if(type1->params.data[i] != type2->params.data[i]){
            return 0;
        }
    }
    for(u32_t i = 0; i < type1->results.size; ++i){
        if(type1->results.data[i] != type2->results.data[i]){
            return 0;
        }
    }
    return 1;
}