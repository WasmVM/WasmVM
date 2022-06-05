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
    funcInst->type = functype;
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