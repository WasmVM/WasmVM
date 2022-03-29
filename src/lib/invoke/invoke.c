/**
 * Copyright 2022 Luis Hsu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include "invoke.h"
#include <error.h>

void invoke(wasm_stack* stack, wasm_store store, u32_t funcaddr){
    const FuncInst* funcInst = store->funcs.data + funcaddr;
    if(funcInst->bodyType == FuncBody_Wasm){
        // Setup frame
        wasm_stack frame = (wasm_stack)malloc_func(sizeof(Stack));
        frame->type = Entry_frame;
        frame->entry.frame.arity = funcInst->type->results.size;
        frame->entry.frame.moduleinst = funcInst->body.wasm.module;
        frame->entry.frame.locals.size = funcInst->type->params.size + funcInst->body.wasm.locals.size;
        frame->entry.frame.locals.data = (Value*)malloc_func(sizeof(Value) * frame->entry.frame.locals.size);
        // Store parameters to frame locals
        for(u32_t i = 0; i < funcInst->type->params.size; ++i){
            wasm_stack entry = *stack;
            if(!entry || entry->type != Entry_value){
                wasmvm_errno = ERROR_type_mis;
                return;
            }
            frame->entry.frame.locals.data[i] = entry->entry.value;
            *stack = entry->next;
            free_func(entry);
        }
        // Push frame
        frame->next = *stack;
        *stack = frame;
        // Push label
        wasm_stack label = (wasm_stack)malloc_func(sizeof(Stack));
        label->type = Entry_label;
        label->entry.label.arity = funcInst->type->results.size;
        label->entry.label.current = (InstrInst*)funcInst->body.wasm.codes.data;
        label->entry.label.end = NULL;
        label->next = *stack;
        *stack = label;
    }else{
        // Invoke hostfunc
        if(funcInst->body.hostcode(stack, store)){
            wasmvm_errno = ERROR_host_func;
        }
    }
}