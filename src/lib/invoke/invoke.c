/**
 * Copyright 2022 Luis Hsu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include "invoke.h"
#include "exec.h"
#include <error.h>
#include <Opcodes.h>

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

void execute(wasm_stack* stack, wasm_store store){
    // Get label & frame
    wasm_stack current_label = *stack;
    current_label->entry.label.last = NULL;
    wasm_stack current_frame = current_label->next;
    current_frame->entry.frame.last = NULL;
    // Run instructions
    while(current_frame){
        switch(current_label->entry.label.current->opcode){
            case Op_end:
                exec_end(&current_label, &current_frame, stack);
            break;
            case Op_local_get:
                exec_local_get(current_label, current_frame, stack, store);
            break;
            case Op_i32_add:
                exec_i32_add(current_label, stack);
            break;
            case Op_i32_sub:
                exec_i32_sub(current_label, stack);
            break;
            case Op_i32_mul:
                exec_i32_mul(current_label, stack);
            break;
            case Op_i32_div_s:
                exec_i32_div_s(current_label, stack);
            break;
            case Op_i32_div_u:
                exec_i32_div_u(current_label, stack);
            break;
            default:
                // Unimplemented
                return;
        }
        if(wasmvm_errno != ERROR_success){
            return;
        }
    }
}