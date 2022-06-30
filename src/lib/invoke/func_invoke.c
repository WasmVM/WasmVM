/**
 * Copyright 2022 Luis Hsu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include <WasmVM.h>
#include <Stack.h>
#include "invoke.h"

values_vector_t func_invoke(wasm_store store, u32_t funcaddr, values_vector_t args){
    values_vector_t results;
    vector_init(results);
    // Get function instance
    if(funcaddr >= store->funcs.size){
        wasmvm_errno = ERROR_unknown_func;
        return results;
    }
    const FuncInst* funcInst = store->funcs.data + funcaddr;
    // Check args
    if(args.size != funcInst->type.params.size){
        wasmvm_errno = ERROR_type_mis;
        return results;
    }
    for(u32_t i = 0; i < funcInst->type.params.size; ++i){
        if(args.data[i].type != funcInst->type.params.data[i]){
            wasmvm_errno = ERROR_type_mis;
            return results;
        }
    }
    // Create Stack
    wasm_stack stack = NULL;
    // Push args
    for(u32_t i = 0; i < args.size; ++i){
        wasm_stack value = (wasm_stack)malloc_func(sizeof(Stack));
        value->type = Entry_value;
        value->entry.value = args.data[i];
        value->next = stack;
        stack = value;
    }
    // Invoke & execute
    invoke(&stack, store, funcaddr);
    execute(&stack, store);
    if(wasmvm_errno != ERROR_success){
        while(stack){
            wasm_stack node = stack;
            stack = stack->next;
            if(node->type == Entry_frame){
                free_func(node->entry.frame.locals.data);
            }
            free_func(node);
        }
        return results;
    }
    // Get return values
    results.size = funcInst->type.results.size;
    if(results.size > 0){
        results.data = (wasm_value*) malloc_func(sizeof(wasm_value) * results.size);
        for(u32_t i = 0; i < results.size; ++i){
            if((stack == NULL) || (stack->type != Entry_value) || (stack->entry.value.type != funcInst->type.results.data[results.size - 1 - i])){
                wasmvm_errno = ERROR_type_mis;
                return results;
            }
            results.data[results.size - 1 - i] = stack->entry.value;
            wasm_stack node = stack;
            stack = stack->next;
            free_func(node);
        }
    }
    return results;
}