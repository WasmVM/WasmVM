/**
 * Copyright 2021 Luis Hsu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include <WasmVM.h>

u32_t func_alloc(wasm_store store, wasm_functype functype, hostfunc_t hostfunc)
{
    // Allocate FuncInst
    u32_t index = store->funcs.size;
    store->funcs.size += 1;
    vector_resize(store->funcs, FuncInst, store->funcs.size);
    // Fill FuncInst
    FuncInst* funcInst = store->funcs.data + index;
    funcInst->bodyType = FuncBody_Host;
    if(functype.params.size > 0) {
        funcInst->type.params.size = functype.params.size;
        funcInst->type.params.data = (ValueType*) malloc_func(sizeof(ValueType) * functype.params.size);
        memcpy_func(funcInst->type.params.data, functype.params.data, sizeof(ValueType) * functype.params.size);
    }
    if(functype.results.size > 0) {
        funcInst->type.results.size = functype.results.size;
        funcInst->type.results.data = (ValueType*) malloc_func(sizeof(ValueType) * functype.results.size);
        memcpy_func(funcInst->type.results.data, functype.results.data, sizeof(ValueType) * functype.results.size);
    }
    funcInst->body.hostcode = hostfunc;
    return index;
}