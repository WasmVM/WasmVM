/**
 * Copyright 2022 Luis Hsu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include "stack.h"
#include <error.h>
#include <Opcodes.h>

typedef struct ValueNode* value_node_t;

struct ValueNode{
    ValueType val;
    value_node_t next;
};

struct CtrlStack {
    ctrl_frame frame;
    ctrl_stack_t next;
};

void push_val(value_stack_t vals, ValueType val){
    value_node_t node = (value_node_t*) malloc_func(sizeof(struct ValueNode));
    node->val = val;
    node->next = vals.data;
    vals.data = node;
    vals.size += 1;
}

ValueType pop_val(value_stack_t vals, ctrl_stack_t ctrls){
    if(!ctrls){
        wasmvm_errno = ERROR_type_mis;
        return Value_index;
    }
    if(vals.size <= ctrls->frame.height){
        if(ctrls->frame.unreachable){
            return Value_unspecified;
        }else{
            wasmvm_errno = ERROR_type_mis;
            return Value_index;
        }
    }
    if((vals.size == 0) || (vals.data == NULL)){
        wasmvm_errno = ERROR_type_mis;
        return Value_index;
    }
    ValueType result = vals.data->val;
    value_node_t node = vals.data;
    vals.data = node->next;
    free_func(node);
    vals.size -= 1;
    return result;
}

ValueType expect_val(value_stack_t vals, ctrl_stack_t ctrls, ValueType expect){
    ValueType actual = pop_val(vals, ctrls);
    if(actual == Value_unspecified){
        return expect;
    }
    if((expect == Value_unspecified) || (actual != expect)){
        return actual;
    }
    wasmvm_errno = ERROR_type_mis;
    return Value_index;
}

void push_ctrl(value_stack_t vals, ctrl_stack_t* ctrls, u16_t opcode, wasm_functype types){
    // Allocate node
    ctrl_stack_t node = (ctrl_stack_t) malloc_func(sizeof(struct CtrlStack));
    node->frame.opcode = opcode;
    node->frame.unreachable = 0;
    node->frame.height = vals.size;
    node->frame.types.params.size = types.params.size;
    node->frame.types.params.data = (ValueType*)malloc_func(sizeof(ValueType) * types.params.size);
    memcpy_func(node->frame.types.params.data, types.params.data, sizeof(ValueType) * types.params.size);
    node->frame.types.results.size = types.results.size;
    node->frame.types.results.data = (ValueType*)malloc_func(sizeof(ValueType) * types.results.size);
    memcpy_func(node->frame.types.results.data, types.results.data, sizeof(ValueType) * types.results.size);
    // Push node
    node->next = *ctrls;
    *ctrls = node;
    (*ctrls)->next = node;
    // Push values
    for(size_t i = 0; i < types.params.size; ++i){
        push_val(vals, types.params.data[i]);
    }
}

ctrl_frame pop_ctrl(value_stack_t vals, ctrl_stack_t* ctrls){
    ctrl_frame frame = {
        .opcode = Op_unreachable,
        .unreachable = 0,
        .types = {
            .params = {.size = 0, .data = NULL},
            .results = {.size = 0, .data = NULL},
        },
        .height = 0,
    };
    if(*ctrls == NULL){
        wasmvm_errno = ERROR_type_mis;
        return frame;
    }
    ctrl_stack_t node = *ctrls;
    frame = node->frame;
    // Pop values
    for(size_t i = 0; i < frame.types.results.size; ++i){
        expect_val(vals, ctrls, frame.types.results.data[i]);
    }
    if(vals.size != frame.height){
        wasmvm_errno = ERROR_type_mis;
        return frame;
    }
    *ctrls = node->next;
    free_func(node);
    return frame;
}

void set_unreachable(value_stack_t vals, ctrl_stack_t ctrls){
    while(vals.size > ctrls->frame.height){
        value_node_t node = vals.data;
        vals.data = node->next;
        free_func(node);
        vals.size -= 1;
    }
    ctrls->frame.unreachable = 1;
}

void free_value_stack(value_stack_t vals){
    while(vals.data){
        value_node_t node = vals.data;
        vals.data = node->next;
        free_func(node);
    }
    vals.size = 0;
}

void free_ctrl_stack(ctrl_stack_t* ctrls){
    while(*ctrls){
        ctrl_stack_t node = *ctrls;
        *ctrls = node->next;
        free_frame(node->frame);
        free_func(node);
    }
}

void free_frame(ctrl_frame frame){
    free_vector(frame.types.params);
    free_vector(frame.types.results);
}