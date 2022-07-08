/**
 * Copyright 2022 Luis Hsu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include "stack.h"
#include <error.h>
#include <Opcodes.h>

typedef struct ValueNode* value_node_t;
typedef struct CtrlNode* ctrl_node_t;

struct ValueNode{
    ValueType val;
    value_node_t next;
};

struct CtrlNode {
    ctrl_frame frame;
    ctrl_node_t next;
};

void push_val(value_stack_t vals, ValueType val){
    value_node_t node = (value_node_t) malloc_func(sizeof(struct ValueNode));
    node->val = val;
    node->next = vals->data;
    vals->data = node;
    vals->size += 1;
}

ValueType pop_val(value_stack_t vals, ctrl_stack_t ctrls){
    if(ctrls->size == 0){
        wasmvm_errno = ERROR_type_mis;
        return Value_index;
    }
    if(vals->size <= ctrls->data->frame.height){
        if(ctrls->data->frame.unreachable){
            return Value_unspecified;
        }else{
            wasmvm_errno = ERROR_type_mis;
            return Value_index;
        }
    }
    if((vals->size == 0) || (vals->data == NULL)){
        wasmvm_errno = ERROR_type_mis;
        return Value_index;
    }
    ValueType result = vals->data->val;
    value_node_t node = vals->data;
    vals->data = node->next;
    free_func(node);
    vals->size -= 1;
    return result;
}

ValueType expect_val(value_stack_t vals, ctrl_stack_t ctrls, ValueType expect){
    ValueType actual = pop_val(vals, ctrls);
    if(actual == Value_unspecified){
        return expect;
    }
    if((expect == Value_unspecified) || (actual == expect)){
        return actual;
    }
    wasmvm_errno = ERROR_type_mis;
    return Value_index;
}

void push_ctrl(value_stack_t vals, ctrl_stack_t ctrls, u16_t opcode, wasm_functype types){
    // Allocate node
    ctrl_node_t node = (ctrl_node_t) malloc_func(sizeof(struct CtrlNode));
    node->frame.opcode = opcode;
    node->frame.unreachable = 0;
    node->frame.height = vals->size;
    node->frame.types.params.size = types.params.size;
    node->frame.types.params.data = (ValueType*)malloc_func(sizeof(ValueType) * types.params.size);
    memcpy_func(node->frame.types.params.data, types.params.data, sizeof(ValueType) * types.params.size);
    node->frame.types.results.size = types.results.size;
    node->frame.types.results.data = (ValueType*)malloc_func(sizeof(ValueType) * types.results.size);
    memcpy_func(node->frame.types.results.data, types.results.data, sizeof(ValueType) * types.results.size);
    // Push node
    node->next = ctrls->data;
    ctrls->data = node;
    ctrls->size += 1;
    // Push values
    for(size_t i = 0; i < types.params.size; ++i){
        push_val(vals, types.params.data[i]);
    }
}

ctrl_frame pop_ctrl(value_stack_t vals, ctrl_stack_t ctrls){
    ctrl_frame frame = {
        .opcode = Op_unreachable,
        .unreachable = 0,
        .types = {
            .params = {.size = 0, .data = NULL},
            .results = {.size = 0, .data = NULL},
        },
        .height = 0,
    };
    if(ctrls->size == 0){
        wasmvm_errno = ERROR_type_mis;
        return frame;
    }
    ctrl_node_t node = ctrls->data;
    frame = node->frame;
    // Pop values
    for(size_t i = 0; i < frame.types.results.size; ++i){
        expect_val(vals, ctrls, frame.types.results.data[i]);
    }
    if(vals->size != frame.height){
        wasmvm_errno = ERROR_type_mis;
        return frame;
    }
    ctrls->data = node->next;
    ctrls->size -= 1;
    free_func(node);
    return frame;
}

void set_unreachable(value_stack_t vals, ctrl_stack_t ctrls){
    while(vals->size > ctrls->data->frame.height){
        value_node_t node = vals->data;
        vals->data = node->next;
        free_func(node);
        vals->size -= 1;
    }
    ctrls->data->frame.unreachable = 1;
}

void free_value_stack(value_stack_t vals){
    while(vals->data){
        value_node_t node = vals->data;
        vals->data = node->next;
        free_func(node);
    }
    vals->size = 0;
}

void free_ctrl_stack(ctrl_stack_t ctrls){
    while(ctrls->data){
        ctrl_node_t node = ctrls->data;
        ctrls->data = node->next;
        free_frame(node->frame);
        free_func(node);
    }
    ctrls->size = 0;
}

void free_frame(ctrl_frame frame){
    free_vector(frame.types.params);
    free_vector(frame.types.results);
}

label_type_t label_types(ctrl_stack_t ctrls, size_t index){
    ctrl_node_t node = ctrls->data;
    for(size_t i = 0; i < index; ++i){
        node = node->next;
    }
    return (node->frame.opcode == Op_loop) ? (label_type_t)&node->frame.types.params : (label_type_t)&node->frame.types.results;
}