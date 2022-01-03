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