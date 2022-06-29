/**
 * Copyright 2022 Luis Hsu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include "exec.h"
#include "invoke.h"
#include <error.h>

typedef enum {
    Float_pos_inf = 0,
    Float_pos_nan = 1,
    Float_neg_inf = 2,
    Float_neg_nan = 3,
    Float_normal = 4,
} Float_kind;

static Float_kind f32_kind(u32_t value){
    if((value & 0x7f800000U) == 0x7f800000U){
        return ((value & 0x7fffffU) != 0) | ((value >> 30) & 2);
    }else{
        return Float_normal;
    }
}

static Float_kind f64_kind(u64_t value){
    if((value & 0x7FF0000000000000LLU) == 0x7FF0000000000000LLU){
        return ((value & 0xfffffffffffffLLU) != 0) | ((value >> 62) & 2);
    }else{
        return Float_normal;
    }
}
static void do_br(u32_t index, wasm_stack* label, wasm_stack* frame, wasm_stack* stack){
    // Args
    wasm_stack *tail = stack;
    // Pop skipped label
    for(u32_t i = 0; i < index; ++i){
        *label = (*label)->entry.label.last;
    }
    // Pop args
    for(u32_t i = 0; i < (*label)->entry.label.arity; ++i){
        tail = &((*tail)->next);
    }
    // Pop label or frame
    wasm_stack cur = *tail, end;
    if((*label)->entry.label.last){
        /** Label end */
        // Concat args
        if((*label)->entry.label.arity != 0){
            *tail = (*label)->next;
        }else{
            *stack = (*label)->next;
        }
        // Pop label
        (*label)->entry.label.last->entry.label.current = (*label)->entry.label.branch;
        end = (*label)->next;
        *label = (*label)->entry.label.last;
    }else{
        /** Frame end */
        // Concat args
        if((*label)->entry.label.arity != 0){
            *tail = (*frame)->next;
        }else{
            *stack = (*frame)->next;
        }
        // Pop frame
        for(*label = (*frame)->next; *label && ((*label)->type != Entry_label); *label = (*label)->next);
        free_vector((*frame)->entry.frame.locals);
        end = (*frame)->next;
        *frame = (*frame)->entry.frame.last;
    }
    
    // Clean remained values
    while(cur != end){
        wasm_stack node = cur;
        cur = cur->next;
        free_func(node);
    }
}

void exec_unreachable(wasm_stack label){
    wasmvm_errno = ERROR_unreachable;
    label->entry.label.current += 1;
}
void exec_nop(wasm_stack label){
    label->entry.label.current += 1;
}
void exec_block(wasm_stack* label, wasm_stack frame, wasm_stack* stack){
    BlockInstrInst* instr = (BlockInstrInst*)(*label)->entry.label.current;
    // Setup label
    wasm_stack new_label = (wasm_stack)malloc_func(sizeof(Stack));
    new_label->type = Entry_label;
    new_label->entry.label.current = (InstrInst*)(instr + 1);
    new_label->entry.label.branch = (InstrInst*)(((byte_t*)instr) + instr->endOffset) + 1;
    new_label->entry.label.last = *label;
    // Args
    wasm_stack args = NULL;
    if(instr->blocktype == Value_index){
        // Pop args
        FuncType* type = frame->entry.frame.moduleinst->types.data + instr->index;
        new_label->entry.label.arity = type->results.size;
        if(type->params.size > 0){
            args = *stack;
            wasm_stack* tail = stack;
            for(u32_t i = 0; i < type->params.size; ++i){
                tail = &((*tail)->next);
            }
            *stack = *tail;
            *tail = new_label;
        }
    }else{
        new_label->entry.label.arity = (instr->blocktype != Value_unspecified);
    }

    // Push label
    (*label)->entry.label.current = new_label->entry.label.branch;
    new_label->next = *stack;
    *label = new_label;

    // Push args
    if(args != NULL){
        *stack = args;
    }else{
        *stack = new_label;
    }
}
void exec_loop(wasm_stack* label, wasm_stack frame, wasm_stack* stack){
    BlockInstrInst* instr = (BlockInstrInst*)(*label)->entry.label.current;
    // Setup label
    wasm_stack new_label = (wasm_stack)malloc_func(sizeof(Stack));
    new_label->type = Entry_label;
    new_label->entry.label.current = (InstrInst*)(instr + 1);
    new_label->entry.label.branch = (InstrInst*)instr;
    new_label->entry.label.last = *label;
    // Args
    wasm_stack args = NULL;
    if(instr->blocktype == Value_index){
        // Pop args
        FuncType* type = frame->entry.frame.moduleinst->types.data + instr->index;
        new_label->entry.label.arity = type->params.size;
        if(type->params.size > 0){
            args = *stack;
            wasm_stack* tail = stack;
            for(u32_t i = 0; i < type->params.size; ++i){
                tail = &((*tail)->next);
            }
            *stack = *tail;
            *tail = new_label;
        }
    }else{
        new_label->entry.label.arity = (instr->blocktype != Value_unspecified);
    }

    // Push label
    (*label)->entry.label.current = ((InstrInst*)(((byte_t*)instr) + instr->endOffset)) + 1;
    new_label->next = *stack;
    *label = new_label;

    // Push args
    if(args != NULL){
        *stack = args;
    }else{
        *stack = new_label;
    }
}
void exec_if(wasm_stack* label, wasm_stack frame, wasm_stack* stack){
    BlockInstrInst* instr = (BlockInstrInst*)(*label)->entry.label.current;
    // Condition
    wasm_stack cond = *stack;
    *stack = (*stack)->next;
    if(cond->entry.value.value.i32 || (instr->brOffset != 0)){
        // Setup label
        wasm_stack new_label = (wasm_stack)malloc_func(sizeof(Stack));
        new_label->type = Entry_label;
        new_label->entry.label.branch = (InstrInst*)(((byte_t*)instr) + instr->endOffset) + 1;
        new_label->entry.label.last = *label;
        // Set jump
        if(cond->entry.value.value.i32){
            new_label->entry.label.current = (InstrInst*)(instr + 1);
        }else{
            new_label->entry.label.current = ((InstrInst*)(((byte_t*)instr) + instr->brOffset)) + 1;
        }
        // Args
        wasm_stack args = NULL;
        if(instr->blocktype == Value_index){
            // Pop args
            FuncType* type = frame->entry.frame.moduleinst->types.data + instr->index;
            new_label->entry.label.arity = type->results.size;
            if(type->params.size > 0){
                args = *stack;
                wasm_stack* tail = stack;
                for(u32_t i = 0; i < type->params.size; ++i){
                    tail = &((*tail)->next);
                }
                *stack = *tail;
                *tail = new_label;
            }
        }else{
            new_label->entry.label.arity = (instr->blocktype != Value_unspecified);
        }
        // Push label
        (*label)->entry.label.current = new_label->entry.label.branch;
        new_label->next = *stack;
        *label = new_label;
        // Push args
        if(args != NULL){
            *stack = args;
        }else{
            *stack = new_label;
        }
    }else{
        (*label)->entry.label.current = ((InstrInst*)(((byte_t*)instr) + instr->endOffset)) + 1;
    }
    free_func(cond);
}
void exec_else(wasm_stack* label, wasm_stack* frame, wasm_stack* stack){
    do_br(0, label, frame, stack);
}
void exec_end(wasm_stack* label, wasm_stack* frame, wasm_stack* stack){
    // Pop results
    wasm_stack res_begin = *stack, *res_end_ptr = NULL;
    while(*stack && (*stack)->type == Entry_value){
        res_end_ptr = &((*stack)->next);
        *stack = (*stack)->next;
    }
    // Pop label
    wasm_stack old_label = *label;
    *stack = old_label->next; // Label must be on top
    // Pop frame if function end
    if((*stack)->type == Entry_frame){
        wasm_stack old_frame = *frame;
        *frame = old_frame->entry.frame.last;
        *stack = old_frame->next;
        for(*label = *stack; *label && ((*label)->type != Entry_label); *label = (*label)->next);
        free_vector(old_frame->entry.frame.locals);
        free_func(old_frame);
    }else{
        *label = old_label->entry.label.last;
    }
    // Push results to stack
    if(res_end_ptr){
        *res_end_ptr = *stack;
        *stack = res_begin;
    }
    // Free old label
    free_func(old_label);
}
void exec_br(wasm_stack* label, wasm_stack* frame, wasm_stack* stack){
    UnaryInstrInst* instr = (UnaryInstrInst*)(*label)->entry.label.current;
    do_br(instr->index, label, frame, stack);
}
void exec_br_if(wasm_stack* label, wasm_stack* frame, wasm_stack* stack){
    UnaryInstrInst* instr = (UnaryInstrInst*)(*label)->entry.label.current;
    // Condition
    wasm_stack cond = *stack;
    *stack = (*stack)->next;
    if(cond->entry.value.value.i32){
        do_br(instr->index, label, frame, stack);
    }else{
        (*label)->entry.label.current = (InstrInst*)(instr + 1);
    }
    free_func(cond);
}
void exec_br_table(wasm_stack* label, wasm_stack* frame, wasm_stack* stack){
    BrTableInstrInst* instr = (BrTableInstrInst*)(*label)->entry.label.current;
    u32_t index = (*stack)->entry.value.value.u32;
    {
        wasm_stack node = *stack;
        *stack = (*stack)->next;
        free_func(node);
    }
    if(index < (instr->size - 1)){
        index = instr->params[index];
    }else{
        index = instr->params[instr->size - 1];
    }
    do_br(index, label, frame, stack);
}
void exec_return(wasm_stack* label, wasm_stack* frame, wasm_stack* stack){
    // Pop results
    wasm_stack res_begin = *stack, *res_end_ptr = NULL;
    for(u32_t i = 0; i < (*frame)->entry.frame.arity; ++i){
        res_end_ptr = &((*stack)->next);
        *stack = (*stack)->next;
    }
    // Pop label & frame
    wasm_stack old_label = *label;
    wasm_stack old_frame = *frame;
    *frame = old_frame->entry.frame.last;
    *stack = old_frame->next; // Frame must be on top
    for(*label = *stack; *label && ((*label)->type != Entry_label); *label = (*label)->next);
    free_vector(old_frame->entry.frame.locals);
    // Push results to stack
    for(wasm_stack cur = (res_end_ptr ? (*res_end_ptr) : res_begin); cur != *stack;){
        wasm_stack node = cur;
        cur = cur->next;
        free_func(node);
    }
    if(res_end_ptr){
        *res_end_ptr = *stack;
        *stack = res_begin;
    }
}
void exec_call(wasm_stack* label, wasm_stack* frame, wasm_stack* stack, wasm_store store){
    UnaryInstrInst* instr = (UnaryInstrInst*)(*label)->entry.label.current;
    (*label)->entry.label.current = (InstrInst*)(instr + 1);
    invoke(stack, store, (*frame)->entry.frame.moduleinst->funcaddrs.data[instr->index]);
    // Update label & frame
    wasm_stack cursor = *stack;
    for(; cursor && (cursor->type != Entry_label); cursor = cursor->next);
    *label = cursor;
    for(; cursor && (cursor->type != Entry_frame); cursor = cursor->next);
    *frame = cursor;
}
void exec_call_indirect(wasm_stack* label, wasm_stack* frame, wasm_stack* stack, wasm_store store){
    BinaryInstrInst* instr = (BinaryInstrInst*)(*label)->entry.label.current;
    TableInst* table = store->tables.data + (*frame)->entry.frame.moduleinst->tableaddrs.data[instr->index2];
    FuncType* expectType = (*frame)->entry.frame.moduleinst->types.data + instr->index1;
    u32_t index = (*stack)->entry.value.value.u32;
    {
        wasm_stack node = *stack;
        *stack = (*stack)->next;
        free_func(node);
    }
    if(index >= table->elem.size){
        wasmvm_errno = ERROR_len_out_of_bound;
        return;
    }
    Ref* ref = table->elem.data + index;
    if(ref->isNull){
        wasmvm_errno = ERROR_indir_call;
        return;
    }
    FuncType* actualType = store->funcs.data[ref->addr].type;
    if(!check_FuncType(expectType, actualType)){
        wasmvm_errno = ERROR_indir_call_mis;
        return;
    }
    (*label)->entry.label.current = (InstrInst*)(instr + 1);
    invoke(stack, store, ref->addr);
    // Update label & frame
    wasm_stack cursor = *stack;
    for(; cursor && (cursor->type != Entry_label); cursor = cursor->next);
    *label = cursor;
    for(; cursor && (cursor->type != Entry_frame); cursor = cursor->next);
    *frame = cursor;
}
void exec_drop(wasm_stack label, wasm_stack* stack){
    wasm_stack value = *stack;
    *stack = value->next;
    free_func(value);
    label->entry.label.current += 1;
}
void exec_select(wasm_stack label, wasm_stack* stack){
    wasm_stack cond = *stack;
    wasm_stack op2 = cond->next;
    wasm_stack op1 = op2->next;
    if(cond->entry.value.value.i32){
        *stack = op1;
        free_func(op2);
    }else{
        op2->next = op1->next;
        *stack = op2;
        free_func(op1);
    }
    free_func(cond);
    label->entry.label.current += 1;
}
void exec_select_t(wasm_stack label, wasm_stack* stack){
    SelectInstrInst* instr = (SelectInstrInst*)label->entry.label.current;
    wasm_stack cond = *stack;
    wasm_stack op2 = cond->next;
    wasm_stack op1 = op2->next;
    if(cond->entry.value.value.i32){
        *stack = op1;
        free_func(op2);
    }else{
        op2->next = op1->next;
        *stack = op2;
        free_func(op1);
    }
    free_func(cond);
    label->entry.label.current = (InstrInst*)(instr + 1);
}
void exec_local_get(wasm_stack label, wasm_stack frame, wasm_stack* stack, wasm_store store){
    UnaryInstrInst* instr = (UnaryInstrInst*)label->entry.label.current;
    wasm_stack value = (wasm_stack)malloc_func(sizeof(Stack));
    value->type = Entry_value;
    value->next = *stack;
    value->entry.value = frame->entry.frame.locals.data[instr->index];
    *stack = value;
    label->entry.label.current = (InstrInst*)(instr + 1);
}
void exec_local_set(wasm_stack label, wasm_stack frame, wasm_stack* stack, wasm_store store){
    UnaryInstrInst* instr = (UnaryInstrInst*)label->entry.label.current;
    wasm_stack value = *stack;
    frame->entry.frame.locals.data[instr->index] = value->entry.value;
    *stack = value->next;
    free_func(value);
    label->entry.label.current = (InstrInst*)(instr + 1);
}
void exec_local_tee(wasm_stack label, wasm_stack frame, wasm_stack* stack, wasm_store store){
    UnaryInstrInst* instr = (UnaryInstrInst*)label->entry.label.current;
    frame->entry.frame.locals.data[instr->index] = (*stack)->entry.value;
    label->entry.label.current = (InstrInst*)(instr + 1);
}
void exec_global_get(wasm_stack label, wasm_stack frame, wasm_stack* stack, wasm_store store){
    UnaryInstrInst* instr = (UnaryInstrInst*)label->entry.label.current;
    GlobalInst* global = store->globals.data + frame->entry.frame.moduleinst->globaladdrs.data[instr->index];
    wasm_stack value = (wasm_stack)malloc_func(sizeof(Stack));
    value->type = Entry_value;
    value->next = *stack;
    value->entry.value = global->val;
    *stack = value;
    label->entry.label.current = (InstrInst*)(instr + 1);
}
void exec_global_set(wasm_stack label, wasm_stack frame, wasm_stack* stack, wasm_store store){
    UnaryInstrInst* instr = (UnaryInstrInst*)label->entry.label.current;
    GlobalInst* global = store->globals.data + frame->entry.frame.moduleinst->globaladdrs.data[instr->index];
    wasm_stack value = *stack;
    global->val = value->entry.value;
    *stack = value->next;
    free_func(value);
    label->entry.label.current = (InstrInst*)(instr + 1);
}
void exec_table_get(wasm_stack* label, wasm_stack* frame, wasm_stack* stack, wasm_store store){
    // TODO:
}
void exec_table_set(wasm_stack* label, wasm_stack* frame, wasm_stack* stack, wasm_store store){
    // TODO:
}
void exec_i32_load(wasm_stack label, wasm_stack frame, wasm_stack* stack, wasm_store store){
    BinaryInstrInst* instr = (BinaryInstrInst*)label->entry.label.current;
    MemInst* mem = store->mems.data + (frame->entry.frame.moduleinst->memaddrs.data[0]);
    wasm_stack value = *stack;
    u32_t ea = value->entry.value.value.u32 + instr->index2;
    if((ea < value->entry.value.value.u32) || (ea < instr->index2) || ((ea + sizeof(i32_t)) > mem->data.size)){
        wasmvm_errno = ERROR_mem_acc_out_bound;
        return;
    }
    value->entry.value.value.i32 = *(i32_t*)(mem->data.data + ea);
    value->entry.value.type = Value_i32;
    label->entry.label.current = (InstrInst*)(instr + 1);
}
void exec_i64_load(wasm_stack label, wasm_stack frame, wasm_stack* stack, wasm_store store){
    BinaryInstrInst* instr = (BinaryInstrInst*)label->entry.label.current;
    MemInst* mem = store->mems.data + (frame->entry.frame.moduleinst->memaddrs.data[0]);
    wasm_stack value = *stack;
    u32_t ea = value->entry.value.value.u32 + instr->index2;
    if((ea < value->entry.value.value.u32) || (ea < instr->index2) || ((ea + sizeof(i64_t)) > mem->data.size)){
        wasmvm_errno = ERROR_mem_acc_out_bound;
        return;
    }
    value->entry.value.value.i64 = *(i64_t*)(mem->data.data + ea);
    value->entry.value.type = Value_i64;
    label->entry.label.current = (InstrInst*)(instr + 1);
}
void exec_f32_load(wasm_stack label, wasm_stack frame, wasm_stack* stack, wasm_store store){
    BinaryInstrInst* instr = (BinaryInstrInst*)label->entry.label.current;
    MemInst* mem = store->mems.data + (frame->entry.frame.moduleinst->memaddrs.data[0]);
    wasm_stack value = *stack;
    u32_t ea = value->entry.value.value.u32 + instr->index2;
    if((ea < value->entry.value.value.u32) || (ea < instr->index2) || ((ea + sizeof(f32_t)) > mem->data.size)){
        wasmvm_errno = ERROR_mem_acc_out_bound;
        return;
    }
    value->entry.value.value.f32 = *(f32_t*)(mem->data.data + ea);
    value->entry.value.type = Value_f32;
    label->entry.label.current = (InstrInst*)(instr + 1);
}
void exec_f64_load(wasm_stack label, wasm_stack frame, wasm_stack* stack, wasm_store store){
    BinaryInstrInst* instr = (BinaryInstrInst*)label->entry.label.current;
    MemInst* mem = store->mems.data + (frame->entry.frame.moduleinst->memaddrs.data[0]);
    wasm_stack value = *stack;
    u32_t ea = value->entry.value.value.u32 + instr->index2;
    if((ea < value->entry.value.value.u32) || (ea < instr->index2) || ((ea + sizeof(f64_t)) > mem->data.size)){
        wasmvm_errno = ERROR_mem_acc_out_bound;
        return;
    }
    value->entry.value.value.f64 = *(f64_t*)(mem->data.data + ea);
    value->entry.value.type = Value_f64;
    label->entry.label.current = (InstrInst*)(instr + 1);
}
void exec_i32_load8_s(wasm_stack label, wasm_stack frame, wasm_stack* stack, wasm_store store){
    BinaryInstrInst* instr = (BinaryInstrInst*)label->entry.label.current;
    MemInst* mem = store->mems.data + (frame->entry.frame.moduleinst->memaddrs.data[0]);
    wasm_stack value = *stack;
    u32_t ea = value->entry.value.value.u32 + instr->index2;
    if((ea < value->entry.value.value.u32) || (ea < instr->index2) || ((ea + sizeof(i8_t)) > mem->data.size)){
        wasmvm_errno = ERROR_mem_acc_out_bound;
        return;
    }
    value->entry.value.value.i32 = (i32_t)*(i8_t*)(mem->data.data + ea);
    value->entry.value.type = Value_i32;
    label->entry.label.current = (InstrInst*)(instr + 1);
}
void exec_i32_load8_u(wasm_stack label, wasm_stack frame, wasm_stack* stack, wasm_store store){
    BinaryInstrInst* instr = (BinaryInstrInst*)label->entry.label.current;
    MemInst* mem = store->mems.data + (frame->entry.frame.moduleinst->memaddrs.data[0]);
    wasm_stack value = *stack;
    u32_t ea = value->entry.value.value.u32 + instr->index2;
    if((ea < value->entry.value.value.u32) || (ea < instr->index2) || ((ea + sizeof(u8_t)) > mem->data.size)){
        wasmvm_errno = ERROR_mem_acc_out_bound;
        return;
    }
    value->entry.value.value.u32 = (u32_t)*(u8_t*)(mem->data.data + ea);
    value->entry.value.type = Value_i32;
    label->entry.label.current = (InstrInst*)(instr + 1);
}
void exec_i32_load16_s(wasm_stack label, wasm_stack frame, wasm_stack* stack, wasm_store store){
    BinaryInstrInst* instr = (BinaryInstrInst*)label->entry.label.current;
    MemInst* mem = store->mems.data + (frame->entry.frame.moduleinst->memaddrs.data[0]);
    wasm_stack value = *stack;
    u32_t ea = value->entry.value.value.u32 + instr->index2;
    if((ea < value->entry.value.value.u32) || (ea < instr->index2) || ((ea + sizeof(i16_t)) > mem->data.size)){
        wasmvm_errno = ERROR_mem_acc_out_bound;
        return;
    }
    value->entry.value.value.i32 = (i32_t)*(i16_t*)(mem->data.data + ea);
    value->entry.value.type = Value_i32;
    label->entry.label.current = (InstrInst*)(instr + 1);
}
void exec_i32_load16_u(wasm_stack label, wasm_stack frame, wasm_stack* stack, wasm_store store){
    BinaryInstrInst* instr = (BinaryInstrInst*)label->entry.label.current;
    MemInst* mem = store->mems.data + (frame->entry.frame.moduleinst->memaddrs.data[0]);
    wasm_stack value = *stack;
    u32_t ea = value->entry.value.value.u32 + instr->index2;
    if((ea < value->entry.value.value.u32) || (ea < instr->index2) || ((ea + sizeof(u16_t)) > mem->data.size)){
        wasmvm_errno = ERROR_mem_acc_out_bound;
        return;
    }
    value->entry.value.value.u32 = (u32_t)*(u16_t*)(mem->data.data + ea);
    value->entry.value.type = Value_i32;
    label->entry.label.current = (InstrInst*)(instr + 1);
}
void exec_i64_load8_s(wasm_stack label, wasm_stack frame, wasm_stack* stack, wasm_store store){
    BinaryInstrInst* instr = (BinaryInstrInst*)label->entry.label.current;
    MemInst* mem = store->mems.data + (frame->entry.frame.moduleinst->memaddrs.data[0]);
    wasm_stack value = *stack;
    u32_t ea = value->entry.value.value.u32 + instr->index2;
    if((ea < value->entry.value.value.u32) || (ea < instr->index2) || ((ea + sizeof(i8_t)) > mem->data.size)){
        wasmvm_errno = ERROR_mem_acc_out_bound;
        return;
    }
    value->entry.value.value.i64 = (i64_t)*(i8_t*)(mem->data.data + ea);
    value->entry.value.type = Value_i64;
    label->entry.label.current = (InstrInst*)(instr + 1);
}
void exec_i64_load8_u(wasm_stack label, wasm_stack frame, wasm_stack* stack, wasm_store store){
    BinaryInstrInst* instr = (BinaryInstrInst*)label->entry.label.current;
    MemInst* mem = store->mems.data + (frame->entry.frame.moduleinst->memaddrs.data[0]);
    wasm_stack value = *stack;
    u32_t ea = value->entry.value.value.u32 + instr->index2;
    if((ea < value->entry.value.value.u32) || (ea < instr->index2) || ((ea + sizeof(u8_t)) > mem->data.size)){
        wasmvm_errno = ERROR_mem_acc_out_bound;
        return;
    }
    value->entry.value.value.u64 = (u64_t)*(u8_t*)(mem->data.data + ea);
    value->entry.value.type = Value_i64;
    label->entry.label.current = (InstrInst*)(instr + 1);
}
void exec_i64_load16_s(wasm_stack label, wasm_stack frame, wasm_stack* stack, wasm_store store){
    BinaryInstrInst* instr = (BinaryInstrInst*)label->entry.label.current;
    MemInst* mem = store->mems.data + (frame->entry.frame.moduleinst->memaddrs.data[0]);
    wasm_stack value = *stack;
    u32_t ea = value->entry.value.value.u32 + instr->index2;
    if((ea < value->entry.value.value.u32) || (ea < instr->index2) || ((ea + sizeof(i16_t)) > mem->data.size)){
        wasmvm_errno = ERROR_mem_acc_out_bound;
        return;
    }
    value->entry.value.value.i64 = (i64_t)*(i16_t*)(mem->data.data + ea);
    value->entry.value.type = Value_i64;
    label->entry.label.current = (InstrInst*)(instr + 1);
}
void exec_i64_load16_u(wasm_stack label, wasm_stack frame, wasm_stack* stack, wasm_store store){
    BinaryInstrInst* instr = (BinaryInstrInst*)label->entry.label.current;
    MemInst* mem = store->mems.data + (frame->entry.frame.moduleinst->memaddrs.data[0]);
    wasm_stack value = *stack;
    u32_t ea = value->entry.value.value.u32 + instr->index2;
    if((ea < value->entry.value.value.u32) || (ea < instr->index2) || ((ea + sizeof(u16_t)) > mem->data.size)){
        wasmvm_errno = ERROR_mem_acc_out_bound;
        return;
    }
    value->entry.value.value.u64 = (u64_t)*(u16_t*)(mem->data.data + ea);
    value->entry.value.type = Value_i64;
    label->entry.label.current = (InstrInst*)(instr + 1);
}
void exec_i64_load32_s(wasm_stack label, wasm_stack frame, wasm_stack* stack, wasm_store store){
    BinaryInstrInst* instr = (BinaryInstrInst*)label->entry.label.current;
    MemInst* mem = store->mems.data + (frame->entry.frame.moduleinst->memaddrs.data[0]);
    wasm_stack value = *stack;
    u32_t ea = value->entry.value.value.u32 + instr->index2;
    if((ea < value->entry.value.value.u32) || (ea < instr->index2) || ((ea + sizeof(i32_t)) > mem->data.size)){
        wasmvm_errno = ERROR_mem_acc_out_bound;
        return;
    }
    value->entry.value.value.i64 = (i64_t)*(i32_t*)(mem->data.data + ea);
    value->entry.value.type = Value_i64;
    label->entry.label.current = (InstrInst*)(instr + 1);
}
void exec_i64_load32_u(wasm_stack label, wasm_stack frame, wasm_stack* stack, wasm_store store){
    BinaryInstrInst* instr = (BinaryInstrInst*)label->entry.label.current;
    MemInst* mem = store->mems.data + (frame->entry.frame.moduleinst->memaddrs.data[0]);
    wasm_stack value = *stack;
    u32_t ea = value->entry.value.value.u32 + instr->index2;
    if((ea < value->entry.value.value.u32) || (ea < instr->index2) || ((ea + sizeof(u32_t)) > mem->data.size)){
        wasmvm_errno = ERROR_mem_acc_out_bound;
        return;
    }
    value->entry.value.value.u64 = (u64_t)*(u32_t*)(mem->data.data + ea);
    value->entry.value.type = Value_i64;
    label->entry.label.current = (InstrInst*)(instr + 1);
}
void exec_i32_store(wasm_stack label, wasm_stack frame, wasm_stack* stack, wasm_store store){
    BinaryInstrInst* instr = (BinaryInstrInst*)label->entry.label.current;
    MemInst* mem = store->mems.data + (frame->entry.frame.moduleinst->memaddrs.data[0]);
    wasm_stack value = *stack;
    wasm_stack address = value->next;
    u32_t ea = address->entry.value.value.u32 + instr->index2;
    if((ea < address->entry.value.value.u32) || (ea < instr->index2) || ((ea + sizeof(i32_t)) > mem->data.size)){
        wasmvm_errno = ERROR_mem_acc_out_bound;
        return;
    }
    *(i32_t*)(mem->data.data + ea) = value->entry.value.value.i32;
    *stack = address->next;
    free_func(address);
    free_func(value);
    label->entry.label.current = (InstrInst*)(instr + 1);
}
void exec_i64_store(wasm_stack label, wasm_stack frame, wasm_stack* stack, wasm_store store){
    BinaryInstrInst* instr = (BinaryInstrInst*)label->entry.label.current;
    MemInst* mem = store->mems.data + (frame->entry.frame.moduleinst->memaddrs.data[0]);
    wasm_stack value = *stack;
    wasm_stack address = value->next;
    u32_t ea = address->entry.value.value.u32 + instr->index2;
    if((ea < address->entry.value.value.u32) || (ea < instr->index2) || ((ea + sizeof(i64_t)) > mem->data.size)){
        wasmvm_errno = ERROR_mem_acc_out_bound;
        return;
    }
    *(i64_t*)(mem->data.data + ea) = value->entry.value.value.i64;
    *stack = address->next;
    free_func(address);
    free_func(value);
    label->entry.label.current = (InstrInst*)(instr + 1);
}
void exec_f32_store(wasm_stack label, wasm_stack frame, wasm_stack* stack, wasm_store store){
    BinaryInstrInst* instr = (BinaryInstrInst*)label->entry.label.current;
    MemInst* mem = store->mems.data + (frame->entry.frame.moduleinst->memaddrs.data[0]);
    wasm_stack value = *stack;
    wasm_stack address = value->next;
    u32_t ea = address->entry.value.value.u32 + instr->index2;
    if((ea < address->entry.value.value.u32) || (ea < instr->index2) || ((ea + sizeof(f32_t)) > mem->data.size)){
        wasmvm_errno = ERROR_mem_acc_out_bound;
        return;
    }
    *(f32_t*)(mem->data.data + ea) = value->entry.value.value.f32;
    *stack = address->next;
    free_func(address);
    free_func(value);
    label->entry.label.current = (InstrInst*)(instr + 1);
}
void exec_f64_store(wasm_stack label, wasm_stack frame, wasm_stack* stack, wasm_store store){
    BinaryInstrInst* instr = (BinaryInstrInst*)label->entry.label.current;
    MemInst* mem = store->mems.data + (frame->entry.frame.moduleinst->memaddrs.data[0]);
    wasm_stack value = *stack;
    wasm_stack address = value->next;
    u32_t ea = address->entry.value.value.u32 + instr->index2;
    if((ea < address->entry.value.value.u32) || (ea < instr->index2) || ((ea + sizeof(f64_t)) > mem->data.size)){
        wasmvm_errno = ERROR_mem_acc_out_bound;
        return;
    }
    *(f64_t*)(mem->data.data + ea) = value->entry.value.value.f64;
    *stack = address->next;
    free_func(address);
    free_func(value);
    label->entry.label.current = (InstrInst*)(instr + 1);
}
void exec_i32_store8(wasm_stack label, wasm_stack frame, wasm_stack* stack, wasm_store store){
    BinaryInstrInst* instr = (BinaryInstrInst*)label->entry.label.current;
    MemInst* mem = store->mems.data + (frame->entry.frame.moduleinst->memaddrs.data[0]);
    wasm_stack value = *stack;
    wasm_stack address = value->next;
    u32_t ea = address->entry.value.value.u32 + instr->index2;
    if((ea < address->entry.value.value.u32) || (ea < instr->index2) || ((ea + sizeof(i8_t)) > mem->data.size)){
        wasmvm_errno = ERROR_mem_acc_out_bound;
        return;
    }
    *(i8_t*)(mem->data.data + ea) = (i8_t)value->entry.value.value.i32;
    *stack = address->next;
    free_func(address);
    free_func(value);
    label->entry.label.current = (InstrInst*)(instr + 1);
}
void exec_i32_store16(wasm_stack label, wasm_stack frame, wasm_stack* stack, wasm_store store){
    BinaryInstrInst* instr = (BinaryInstrInst*)label->entry.label.current;
    MemInst* mem = store->mems.data + (frame->entry.frame.moduleinst->memaddrs.data[0]);
    wasm_stack value = *stack;
    wasm_stack address = value->next;
    u32_t ea = address->entry.value.value.u32 + instr->index2;
    if((ea < address->entry.value.value.u32) || (ea < instr->index2) || ((ea + sizeof(i16_t)) > mem->data.size)){
        wasmvm_errno = ERROR_mem_acc_out_bound;
        return;
    }
    *(i16_t*)(mem->data.data + ea) = (i16_t)value->entry.value.value.i32;
    *stack = address->next;
    free_func(address);
    free_func(value);
    label->entry.label.current = (InstrInst*)(instr + 1);
}
void exec_i64_store8(wasm_stack label, wasm_stack frame, wasm_stack* stack, wasm_store store){
    BinaryInstrInst* instr = (BinaryInstrInst*)label->entry.label.current;
    MemInst* mem = store->mems.data + (frame->entry.frame.moduleinst->memaddrs.data[0]);
    wasm_stack value = *stack;
    wasm_stack address = value->next;
    u32_t ea = address->entry.value.value.u32 + instr->index2;
    if((ea < address->entry.value.value.u32) || (ea < instr->index2) || ((ea + sizeof(i8_t)) > mem->data.size)){
        wasmvm_errno = ERROR_mem_acc_out_bound;
        return;
    }
    *(i8_t*)(mem->data.data + ea) = (i8_t)value->entry.value.value.i64;
    *stack = address->next;
    free_func(address);
    free_func(value);
    label->entry.label.current = (InstrInst*)(instr + 1);
}
void exec_i64_store16(wasm_stack label, wasm_stack frame, wasm_stack* stack, wasm_store store){
    BinaryInstrInst* instr = (BinaryInstrInst*)label->entry.label.current;
    MemInst* mem = store->mems.data + (frame->entry.frame.moduleinst->memaddrs.data[0]);
    wasm_stack value = *stack;
    wasm_stack address = value->next;
    u32_t ea = address->entry.value.value.u32 + instr->index2;
    if((ea < address->entry.value.value.u32) || (ea < instr->index2) || ((ea + sizeof(i16_t)) > mem->data.size)){
        wasmvm_errno = ERROR_mem_acc_out_bound;
        return;
    }
    *(i16_t*)(mem->data.data + ea) = (i16_t)value->entry.value.value.i64;
    *stack = address->next;
    free_func(address);
    free_func(value);
    label->entry.label.current = (InstrInst*)(instr + 1);
}
void exec_i64_store32(wasm_stack label, wasm_stack frame, wasm_stack* stack, wasm_store store){
    BinaryInstrInst* instr = (BinaryInstrInst*)label->entry.label.current;
    MemInst* mem = store->mems.data + (frame->entry.frame.moduleinst->memaddrs.data[0]);
    wasm_stack value = *stack;
    wasm_stack address = value->next;
    u32_t ea = address->entry.value.value.u32 + instr->index2;
    if((ea < address->entry.value.value.u32) || (ea < instr->index2) || ((ea + sizeof(i32_t)) > mem->data.size)){
        wasmvm_errno = ERROR_mem_acc_out_bound;
        return;
    }
    *(i32_t*)(mem->data.data + ea) = (i32_t)value->entry.value.value.i64;
    *stack = address->next;
    free_func(address);
    free_func(value);
    label->entry.label.current = (InstrInst*)(instr + 1);
}
void exec_memory_size(wasm_stack label, wasm_stack frame, wasm_stack* stack, wasm_store store){
    wasm_stack value = (wasm_stack)malloc_func(sizeof(Stack));
    MemInst* mem = store->mems.data + (frame->entry.frame.moduleinst->memaddrs.data[0]);
    value->type = Entry_value;
    value->next = *stack;
    value->entry.value.type = Value_i32;
    value->entry.value.value.u32 = mem->data.size / page_size;
    *stack = value;
    label->entry.label.current += 1;
}
void exec_memory_grow(wasm_stack label, wasm_stack frame, wasm_stack* stack, wasm_store store){
    wasm_stack value = *stack;
    MemInst* mem = store->mems.data + (frame->entry.frame.moduleinst->memaddrs.data[0]);
    u32_t memsize = mem->data.size / page_size;
    u32_t newsize = memsize + value->entry.value.value.u32;
    value->entry.value.value.u32 = memsize;
    if((newsize > 65536) || ((mem->max > 0) && (newsize > mem->max))){
        value->entry.value.value.i32 = -1;
    }else{
        vector_resize(mem->data, byte_t, (newsize * page_size));
        if(mem->data.data == NULL){
            value->entry.value.value.i32 = -1;
        }
        u64_t* data = (u64_t*)(mem->data.data + mem->data.size);
        for(u32_t i = 0; i < ((newsize - memsize) * (page_size / sizeof(u64_t))); ++i){
            data[i] = 0;
        } 
        mem->data.size = newsize * page_size;
    }
    label->entry.label.current += 1;
}
void exec_const(wasm_stack label, wasm_stack* stack){
    ConstInstrInst* instr = (ConstInstrInst*)label->entry.label.current;
    wasm_stack value = (wasm_stack)malloc_func(sizeof(Stack));
    value->type = Entry_value;
    value->next = *stack;
    value->entry.value = instr->constant;
    *stack = value;
    label->entry.label.current = (InstrInst*)(instr + 1);
}
void exec_i32_eqz(wasm_stack label, wasm_stack* stack){
    wasm_stack value = *stack;
    value->entry.value.value.i32 = (value->entry.value.value.i32 == 0);
    label->entry.label.current += 1;
}
void exec_i32_eq(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    value1->entry.value.value.i32 = (value1->entry.value.value.i32 == value2->entry.value.value.i32);
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_i32_ne(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    value1->entry.value.value.i32 = (value1->entry.value.value.i32 != value2->entry.value.value.i32);
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_i32_lt_s(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    value1->entry.value.value.i32 = (value1->entry.value.value.i32 < value2->entry.value.value.i32);
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_i32_lt_u(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    value1->entry.value.value.i32 = (value1->entry.value.value.u32 < value2->entry.value.value.u32);
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_i32_gt_s(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    value1->entry.value.value.i32 = (value1->entry.value.value.i32 > value2->entry.value.value.i32);
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_i32_gt_u(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    value1->entry.value.value.i32 = (value1->entry.value.value.u32 > value2->entry.value.value.u32);
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_i32_le_s(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    value1->entry.value.value.i32 = (value1->entry.value.value.i32 <= value2->entry.value.value.i32);
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_i32_le_u(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    value1->entry.value.value.i32 = (value1->entry.value.value.u32 <= value2->entry.value.value.u32);
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_i32_ge_s(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    value1->entry.value.value.i32 = (value1->entry.value.value.i32 >= value2->entry.value.value.i32);
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_i32_ge_u(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    value1->entry.value.value.i32 = (value1->entry.value.value.u32 >= value2->entry.value.value.u32);
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_i64_eqz(wasm_stack label, wasm_stack* stack){
    wasm_stack value = *stack;
    value->entry.value.value.i32 = (value->entry.value.value.i64 == 0);
    value->entry.value.type = Value_i32;
    label->entry.label.current += 1;
}
void exec_i64_eq(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    value1->entry.value.value.i32 = (value1->entry.value.value.i64 == value2->entry.value.value.i64);
    value1->entry.value.type = Value_i32;
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_i64_ne(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    value1->entry.value.value.i32 = (value1->entry.value.value.i64 != value2->entry.value.value.i64);
    value1->entry.value.type = Value_i32;
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_i64_lt_s(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    value1->entry.value.value.i32 = (value1->entry.value.value.i64 < value2->entry.value.value.i64);
    value1->entry.value.type = Value_i32;
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_i64_lt_u(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    value1->entry.value.value.i32 = (value1->entry.value.value.u64 < value2->entry.value.value.u64);
    value1->entry.value.type = Value_i32;
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_i64_gt_s(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    value1->entry.value.value.i32 = (value1->entry.value.value.i64 > value2->entry.value.value.i64);
    value1->entry.value.type = Value_i32;
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_i64_gt_u(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    value1->entry.value.value.i32 = (value1->entry.value.value.u64 > value2->entry.value.value.u64);
    value1->entry.value.type = Value_i32;
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_i64_le_s(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    value1->entry.value.value.i32 = (value1->entry.value.value.i64 <= value2->entry.value.value.i64);
    value1->entry.value.type = Value_i32;
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_i64_le_u(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    value1->entry.value.value.i32 = (value1->entry.value.value.u64 <= value2->entry.value.value.u64);
    value1->entry.value.type = Value_i32;
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_i64_ge_s(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    value1->entry.value.value.i32 = (value1->entry.value.value.i64 >= value2->entry.value.value.i64);
    value1->entry.value.type = Value_i32;
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_i64_ge_u(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    value1->entry.value.value.i32 = (value1->entry.value.value.u64 >= value2->entry.value.value.u64);
    value1->entry.value.type = Value_i32;
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_f32_eq(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    if((f32_kind(value1->entry.value.value.u32) & 1) || (f32_kind(value2->entry.value.value.u32) & 1)){
        value1->entry.value.value.i32 = 0;
    }else{
        value1->entry.value.value.i32 = ((value1->entry.value.value.u32 == value2->entry.value.value.u32) || ((value1->entry.value.value.u32 | value2->entry.value.value.u32) == 0x80000000));
    }
    value1->entry.value.type = Value_i32;
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_f32_ne(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    if((f32_kind(value1->entry.value.value.u32) & 1) || (f32_kind(value2->entry.value.value.u32) & 1)){
        value1->entry.value.value.i32 = 1;
    }else{
        value1->entry.value.value.i32 = !((value1->entry.value.value.u32 == value2->entry.value.value.u32) || ((value1->entry.value.value.u32 | value2->entry.value.value.u32) == 0x80000000));
    }
    value1->entry.value.type = Value_i32;
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_f32_lt(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    Float_kind kind1 = f32_kind(value1->entry.value.value.u32);
    Float_kind kind2 = f32_kind(value2->entry.value.value.u32);
    if((kind1 & 1) || (kind2 & 1) || (kind1 == Float_pos_inf) || (kind2 == Float_neg_inf)){
        value1->entry.value.value.i32 = 0;
    }else if((kind1 == Float_neg_inf) || (kind2 == Float_pos_inf)){
        value1->entry.value.value.i32 = 1;
    }else{
        value1->entry.value.value.i32 = (((value1->entry.value.value.i32 >= 0 && value1->entry.value.value.i32 < value2->entry.value.value.i32) || (value1->entry.value.value.i32 < 0 && value1->entry.value.value.u32 > value2->entry.value.value.u32)) && ((value1->entry.value.value.u32 | value2->entry.value.value.u32) != 0x80000000));
    }
    value1->entry.value.type = Value_i32;
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_f32_gt(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    Float_kind kind1 = f32_kind(value1->entry.value.value.u32);
    Float_kind kind2 = f32_kind(value2->entry.value.value.u32);
    if((kind1 & 1) || (kind2 & 1) || (kind1 == Float_neg_inf) || (kind2 == Float_pos_inf)){
        value1->entry.value.value.i32 = 0;
    }else if((kind1 == Float_pos_inf) || (kind2 == Float_neg_inf)){
        value1->entry.value.value.i32 = 1;
    }else{
        value1->entry.value.value.i32 = (((value1->entry.value.value.i32 >= 0 && value1->entry.value.value.i32 > value2->entry.value.value.i32) || (value1->entry.value.value.i32 < 0 && value1->entry.value.value.u32 < value2->entry.value.value.u32)) && ((value1->entry.value.value.u32 | value2->entry.value.value.u32) != 0x80000000));
    }
    value1->entry.value.type = Value_i32;
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_f32_le(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    Float_kind kind1 = f32_kind(value1->entry.value.value.u32);
    Float_kind kind2 = f32_kind(value2->entry.value.value.u32);
    if((kind1 & 1) || (kind2 & 1) || (kind1 == Float_pos_inf && kind2 != Float_pos_inf) || (kind2 == Float_neg_inf && kind1 != Float_neg_inf)){
        value1->entry.value.value.i32 = 0;
    }else if((kind1 == Float_neg_inf) || (kind2 == Float_pos_inf)){
        value1->entry.value.value.i32 = 1;
    }else{
        value1->entry.value.value.i32 = (((value1->entry.value.value.i32 >= 0 && value1->entry.value.value.i32 <= value2->entry.value.value.i32) || (value1->entry.value.value.i32 < 0 && value1->entry.value.value.u32 >= value2->entry.value.value.u32)) || ((value1->entry.value.value.u32 | value2->entry.value.value.u32) == 0x80000000));
    }
    value1->entry.value.type = Value_i32;
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_f32_ge(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    Float_kind kind1 = f32_kind(value1->entry.value.value.u32);
    Float_kind kind2 = f32_kind(value2->entry.value.value.u32);
    if((kind1 & 1) || (kind2 & 1) || (kind1 == Float_neg_inf && kind2 != Float_neg_inf) || (kind2 == Float_pos_inf && kind1 != Float_pos_inf)){
        value1->entry.value.value.i32 = 0;
    }else if((kind1 == Float_pos_inf) || (kind2 == Float_neg_inf)){
        value1->entry.value.value.i32 = 1;
    }else{
        value1->entry.value.value.i32 = (((value1->entry.value.value.i32 >= 0 && value1->entry.value.value.i32 >= value2->entry.value.value.i32) || (value1->entry.value.value.i32 < 0 && value1->entry.value.value.u32 <= value2->entry.value.value.u32)) || ((value1->entry.value.value.u32 | value2->entry.value.value.u32) == 0x80000000));
    }
    value1->entry.value.type = Value_i32;
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_f64_eq(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    if((f64_kind(value1->entry.value.value.u64) & 1) || (f64_kind(value2->entry.value.value.u64) & 1)){
        value1->entry.value.value.i32 = 0;
    }else{
        value1->entry.value.value.i32 = ((value1->entry.value.value.u64 == value2->entry.value.value.u64) || ((value1->entry.value.value.u64 | value2->entry.value.value.u64) == 0x8000000000000000LLU));
    }
    value1->entry.value.type = Value_i32;
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_f64_ne(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    if((f64_kind(value1->entry.value.value.u64) & 1) || (f64_kind(value2->entry.value.value.u64) & 1)){
        value1->entry.value.value.i32 = 1;
    }else{
        value1->entry.value.value.i32 = !((value1->entry.value.value.u64 == value2->entry.value.value.u64) || ((value1->entry.value.value.u64 | value2->entry.value.value.u64) == 0x8000000000000000LLU));
    }
    value1->entry.value.type = Value_i32;
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_f64_lt(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    Float_kind kind1 = f64_kind(value1->entry.value.value.u64);
    Float_kind kind2 = f64_kind(value2->entry.value.value.u64);
    if((kind1 & 1) || (kind2 & 1) || (kind1 == Float_pos_inf) || (kind2 == Float_neg_inf)){
        value1->entry.value.value.i32 = 0;
    }else if((kind1 == Float_neg_inf) || (kind2 == Float_pos_inf)){
        value1->entry.value.value.i32 = 1;
    }else{
        value1->entry.value.value.i32 = (((value1->entry.value.value.i64 >= 0 && value1->entry.value.value.i64 < value2->entry.value.value.i64) || (value1->entry.value.value.i64 < 0 && value1->entry.value.value.u64 > value2->entry.value.value.u64)) && ((value1->entry.value.value.u64 | value2->entry.value.value.u64) != 0x8000000000000000LLU));
    }
    value1->entry.value.type = Value_i32;
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_f64_gt(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    Float_kind kind1 = f64_kind(value1->entry.value.value.u64);
    Float_kind kind2 = f64_kind(value2->entry.value.value.u64);
    if((kind1 & 1) || (kind2 & 1) || (kind1 == Float_neg_inf) || (kind2 == Float_pos_inf)){
        value1->entry.value.value.i32 = 0;
    }else if((kind1 == Float_pos_inf) || (kind2 == Float_neg_inf)){
        value1->entry.value.value.i32 = 1;
    }else{
        value1->entry.value.value.i32 = (((value1->entry.value.value.i64 >= 0 && value1->entry.value.value.i64 > value2->entry.value.value.i64) || (value1->entry.value.value.i64 < 0 && value1->entry.value.value.u64 < value2->entry.value.value.u64)) && ((value1->entry.value.value.u64 | value2->entry.value.value.u64) != 0x8000000000000000LLU));
    }
    value1->entry.value.type = Value_i32;
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_f64_le(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    Float_kind kind1 = f64_kind(value1->entry.value.value.u64);
    Float_kind kind2 = f64_kind(value2->entry.value.value.u64);
    if((kind1 & 1) || (kind2 & 1) || (kind1 == Float_pos_inf && kind2 != Float_pos_inf) || (kind2 == Float_neg_inf && kind1 != Float_neg_inf)){
        value1->entry.value.value.i32 = 0;
    }else if((kind1 == Float_neg_inf) || (kind2 == Float_pos_inf)){
        value1->entry.value.value.i32 = 1;
    }else{
        value1->entry.value.value.i32 = (((value1->entry.value.value.i64 >= 0 && value1->entry.value.value.i64 <= value2->entry.value.value.i64) || (value1->entry.value.value.i64 < 0 && value1->entry.value.value.u64 >= value2->entry.value.value.u64)) || ((value1->entry.value.value.u64 | value2->entry.value.value.u64) == 0x8000000000000000LLU));
    }
    value1->entry.value.type = Value_i32;
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_f64_ge(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    Float_kind kind1 = f64_kind(value1->entry.value.value.u64);
    Float_kind kind2 = f64_kind(value2->entry.value.value.u64);
    if((kind1 & 1) || (kind2 & 1) || (kind1 == Float_neg_inf && kind2 != Float_neg_inf) || (kind2 == Float_pos_inf && kind1 != Float_pos_inf)){
        value1->entry.value.value.i32 = 0;
    }else if((kind1 == Float_pos_inf) || (kind2 == Float_neg_inf)){
        value1->entry.value.value.i32 = 1;
    }else{
        value1->entry.value.value.i32 = (((value1->entry.value.value.i64 >= 0 && value1->entry.value.value.i64 >= value2->entry.value.value.i64) || (value1->entry.value.value.i64 < 0 && value1->entry.value.value.u64 <= value2->entry.value.value.u64)) || ((value1->entry.value.value.u64 | value2->entry.value.value.u64) == 0x8000000000000000LLU));
    }
    value1->entry.value.type = Value_i32;
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_i32_clz(wasm_stack label, wasm_stack* stack){
    wasm_stack value = *stack;
    if(value->entry.value.value.i32 == 0){
        value->entry.value.value.i32 = 32;
    }else{
        u32_t count = 0;
        if(!(value->entry.value.value.i32 & 0xffff0000)){
            count += 16;
            value->entry.value.value.i32 <<= 16;
        }
        if(!(value->entry.value.value.i32 & 0xff000000)){
            count += 8;
            value->entry.value.value.i32 <<= 8;
        }
        if(!(value->entry.value.value.i32 & 0xf0000000)){
            count += 4;
            value->entry.value.value.i32 <<= 4;
        }
        if(!(value->entry.value.value.i32 & 0xc0000000)){
            count += 2;
            value->entry.value.value.i32 <<= 2;
        }
        if(!(value->entry.value.value.i32 & 0x80000000)){
            count += 1;
            value->entry.value.value.i32 <<= 1;
        }
        value->entry.value.value.u32 = count;
    }
    label->entry.label.current += 1;
}
void exec_i32_ctz(wasm_stack label, wasm_stack* stack){
    wasm_stack value = *stack;
    if(value->entry.value.value.i32 == 0){
        value->entry.value.value.i32 = 32;
    }else{
        u32_t count = 0;
        if(!(value->entry.value.value.u32 & 0xffff)){
            count += 16;
            value->entry.value.value.u32 >>= 16;
        }
        if(!(value->entry.value.value.u32 & 0xff)){
            count += 8;
            value->entry.value.value.u32 >>= 8;
        }
        if(!(value->entry.value.value.i32 & 0xf)){
            count += 4;
            value->entry.value.value.u32 >>= 4;
        }
        if(!(value->entry.value.value.u32 & 0x3)){
            count += 2;
            value->entry.value.value.u32 >>= 2;
        }
        if(!(value->entry.value.value.u32 & 0x1)){
            count += 1;
            value->entry.value.value.u32 >>= 1;
        }
        value->entry.value.value.u32 = count;
    }
    label->entry.label.current += 1;
}
void exec_i32_popcnt(wasm_stack label, wasm_stack* stack){
    wasm_stack value = *stack;
    value->entry.value.value.u32 = value->entry.value.value.u32 - ((value->entry.value.value.u32 >> 1) & 033333333333) - ((value->entry.value.value.u32 >> 2) & 011111111111);
    value->entry.value.value.u32 = ((value->entry.value.value.u32 + (value->entry.value.value.u32 >> 3)) & 030707070707) % 63;
    label->entry.label.current += 1;
}
void exec_i32_add(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    value1->entry.value.value.i32 += value2->entry.value.value.i32;
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_i32_sub(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    value1->entry.value.value.i32 -= value2->entry.value.value.i32;
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_i32_mul(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    value1->entry.value.value.i32 *= value2->entry.value.value.i32;
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_i32_div_s(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    if(value2->entry.value.value.i32 == 0){
        wasmvm_errno = ERROR_div_zero;
        return;
    }
    if((value1->entry.value.value.i32 == 0x80000000) && (value2->entry.value.value.i32 == -1)){
        wasmvm_errno = ERROR_int_overflow;
        return;
    }
    value1->entry.value.value.i32 /= value2->entry.value.value.i32;
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_i32_div_u(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    if(value2->entry.value.value.u32 == 0){
        wasmvm_errno = ERROR_div_zero;
        return;
    }
    value1->entry.value.value.u32 /= value2->entry.value.value.u32;
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_i32_rem_s(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    if(value2->entry.value.value.i32 == 0){
        wasmvm_errno = ERROR_div_zero;
        return;
    }
    if(value2->entry.value.value.i32 < 0){
        value2->entry.value.value.i32 = -value2->entry.value.value.i32;
    }
    i32_t result = ((value1->entry.value.value.i32 <= 0) ? (-value1->entry.value.value.i32) : value1->entry.value.value.i32) % value2->entry.value.value.i32;
    value1->entry.value.value.i32 = (value1->entry.value.value.i32 <= 0) ? -result : result;
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_i32_rem_u(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    if(value2->entry.value.value.u32 == 0){
        wasmvm_errno = ERROR_div_zero;
        return;
    }
    value1->entry.value.value.u32 %= value2->entry.value.value.u32;
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_i32_and(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    value1->entry.value.value.i32 &= value2->entry.value.value.i32;
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_i32_or(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    value1->entry.value.value.i32 |= value2->entry.value.value.i32;
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_i32_xor(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    value1->entry.value.value.i32 ^= value2->entry.value.value.i32;
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_i32_shl(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    value1->entry.value.value.i32 <<= value2->entry.value.value.i32;
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_i32_shr_s(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    value1->entry.value.value.i32 >>= value2->entry.value.value.i32;
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_i32_shr_u(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    value1->entry.value.value.u32 >>= value2->entry.value.value.u32;
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_i32_rotl(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    value2->entry.value.value.u32 %= 32U;
    value1->entry.value.value.u32 = (value1->entry.value.value.u32 << value2->entry.value.value.u32) | (value1->entry.value.value.u32 >> (32U - value2->entry.value.value.u32));
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_i32_rotr(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    value2->entry.value.value.u32 %= 32U;
    value1->entry.value.value.u32 = (value1->entry.value.value.u32 >> value2->entry.value.value.u32) | (value1->entry.value.value.u32 << (32U - value2->entry.value.value.u32));
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_i64_clz(wasm_stack label, wasm_stack* stack){
    wasm_stack value = *stack;
    if(value->entry.value.value.i64 == 0){
        value->entry.value.value.i64 = 64;
    }else{
        u64_t count = 0;
        if(!(value->entry.value.value.i64 & 0xffffffff00000000LLU)){
            count += 32;
            value->entry.value.value.i64 <<= 32;
        }
        if(!(value->entry.value.value.i64 & 0xffff000000000000LLU)){
            count += 16;
            value->entry.value.value.i64 <<= 16;
        }
        if(!(value->entry.value.value.i64 & 0xff00000000000000LLU)){
            count += 8;
            value->entry.value.value.i64 <<= 8;
        }
        if(!(value->entry.value.value.i64 & 0xf000000000000000LLU)){
            count += 4;
            value->entry.value.value.i64 <<= 4;
        }
        if(!(value->entry.value.value.i64 & 0xc000000000000000LLU)){
            count += 2;
            value->entry.value.value.i64 <<= 2;
        }
        if(!(value->entry.value.value.i64 & 0x8000000000000000LLU)){
            count += 1;
            value->entry.value.value.i64 <<= 1;
        }
        value->entry.value.value.u64 = count;
    }
    label->entry.label.current += 1;
}
void exec_i64_ctz(wasm_stack label, wasm_stack* stack){
    wasm_stack value = *stack;
    if(value->entry.value.value.i64 == 0){
        value->entry.value.value.i64 = 64;
    }else{
        u64_t count = 0;
        if(!(value->entry.value.value.u64 & 0xffffffffLLU)){
            count += 32;
            value->entry.value.value.u64 >>= 32;
        }
        if(!(value->entry.value.value.u64 & 0xffffLLU)){
            count += 16;
            value->entry.value.value.u64 >>= 16;
        }
        if(!(value->entry.value.value.u64 & 0xffLLU)){
            count += 8;
            value->entry.value.value.u64 >>= 8;
        }
        if(!(value->entry.value.value.i64 & 0xfLLU)){
            count += 4;
            value->entry.value.value.u64 >>= 4;
        }
        if(!(value->entry.value.value.u64 & 0x3LLU)){
            count += 2;
            value->entry.value.value.u64 >>= 2;
        }
        if(!(value->entry.value.value.u64 & 0x1LLU)){
            count += 1;
            value->entry.value.value.u64 >>= 1;
        }
        value->entry.value.value.u64 = count;
    }
    label->entry.label.current += 1;
}
void exec_i64_popcnt(wasm_stack label, wasm_stack* stack){
    wasm_stack value = *stack;
    value->entry.value.value.u64 -= ((value->entry.value.value.u64 >> 1) & 0x5555555555555555LLU);
    value->entry.value.value.u64 = (value->entry.value.value.u64 & 0x3333333333333333LLU) + (value->entry.value.value.u64 >> 2 & 0x3333333333333333LLU);
    value->entry.value.value.u64 = ((value->entry.value.value.u64 + (value->entry.value.value.u64 >> 4)) & 0xf0f0f0f0f0f0f0fLLU) * 0x101010101010101LLU >> 56;
    label->entry.label.current += 1;
}
void exec_i64_add(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    value1->entry.value.value.i64 += value2->entry.value.value.i64;
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_i64_sub(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    value1->entry.value.value.i64 -= value2->entry.value.value.i64;
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_i64_mul(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    value1->entry.value.value.i64 *= value2->entry.value.value.i64;
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_i64_div_s(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    if(value2->entry.value.value.i64 == 0){
        wasmvm_errno = ERROR_div_zero;
        return;
    }
    if((value1->entry.value.value.i64 == 0x8000000000000000LLU) && (value2->entry.value.value.i64 == -1)){
        wasmvm_errno = ERROR_int_overflow;
        return;
    }
    value1->entry.value.value.i64 /= value2->entry.value.value.i64;
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_i64_div_u(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    if(value2->entry.value.value.u64 == 0){
        wasmvm_errno = ERROR_div_zero;
        return;
    }
    value1->entry.value.value.u64 /= value2->entry.value.value.u64;
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_i64_rem_s(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    if(value2->entry.value.value.i64 == 0){
        wasmvm_errno = ERROR_div_zero;
        return;
    }
    if(value2->entry.value.value.i64 < 0){
        value2->entry.value.value.i64 = -value2->entry.value.value.i64;
    }
    i64_t result = ((value1->entry.value.value.i64 <= 0) ? (-value1->entry.value.value.i64) : value1->entry.value.value.i64) % value2->entry.value.value.i64;
    value1->entry.value.value.i64 = (value1->entry.value.value.i64 <= 0) ? -result : result;
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_i64_rem_u(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    if(value2->entry.value.value.u64 == 0){
        wasmvm_errno = ERROR_div_zero;
        return;
    }
    value1->entry.value.value.u64 %= value2->entry.value.value.u64;
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_i64_and(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    value1->entry.value.value.i64 &= value2->entry.value.value.i64;
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_i64_or(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    value1->entry.value.value.i64 |= value2->entry.value.value.i64;
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_i64_xor(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    value1->entry.value.value.i64 ^= value2->entry.value.value.i64;
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_i64_shl(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    value1->entry.value.value.i64 <<= value2->entry.value.value.i64;
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_i64_shr_s(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    value1->entry.value.value.i64 >>= value2->entry.value.value.i64;
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_i64_shr_u(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    value1->entry.value.value.u64 >>= value2->entry.value.value.u64;
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_i64_rotl(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    value2->entry.value.value.u64 %= 64LLU;
    value1->entry.value.value.u64 = (value1->entry.value.value.u64 << value2->entry.value.value.u64) | (value1->entry.value.value.u64 >> (64LLU - value2->entry.value.value.u64));
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_i64_rotr(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    value2->entry.value.value.u64 %= 64LLU;
    value1->entry.value.value.u64 = (value1->entry.value.value.u64 >> value2->entry.value.value.u64) | (value1->entry.value.value.u64 << (64LLU - value2->entry.value.value.u64));
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_f32_abs(wasm_stack label, wasm_stack* stack){
    wasm_stack value = *stack;
    value->entry.value.value.u32 &= 0x7fffffff;
    label->entry.label.current += 1;
}
void exec_f32_neg(wasm_stack label, wasm_stack* stack){
    wasm_stack value = *stack;
    value->entry.value.value.u32 ^= 0x80000000;
    label->entry.label.current += 1;
}
void exec_f32_ceil(wasm_stack label, wasm_stack* stack){
    wasm_stack value = *stack;
    Float_kind kind = f32_kind(value->entry.value.value.u32);
    if(kind & 1){
        value->entry.value.value.u32 |= 0x7fc00000;
    }else if(kind == Float_normal){
        if((value->entry.value.value.f32 != (i32_t)value->entry.value.value.f32) && (((value->entry.value.value.u32 & 0x7f800000) >> 23) < 150)){
            if(value->entry.value.value.f32 < 0){
                value->entry.value.value.f32 = (i32_t)value->entry.value.value.f32;
                value->entry.value.value.u32 |= 0x80000000;
            }else{
                value->entry.value.value.f32 = (i32_t)value->entry.value.value.f32 + 1;
            }
        }
    }
    label->entry.label.current += 1;
}
void exec_f32_floor(wasm_stack label, wasm_stack* stack){
    wasm_stack value = *stack;
    Float_kind kind = f32_kind(value->entry.value.value.u32);
    if(kind == Float_normal){
        if((value->entry.value.value.f32 != (i32_t)value->entry.value.value.f32) && (((value->entry.value.value.u32 & 0x7f800000) >> 23) < 150)){
            if(value->entry.value.value.f32 < 0){
                value->entry.value.value.f32 = (i32_t)value->entry.value.value.f32 - 1;
            }else{
                value->entry.value.value.f32 = (i32_t)value->entry.value.value.f32;
                value->entry.value.value.u32 &= 0x7fffffff;
            }
        }
    }else if(kind & 1){
        value->entry.value.value.u32 |= 0x7fc00000;
    }
    label->entry.label.current += 1;
}
void exec_f32_trunc(wasm_stack label, wasm_stack* stack){
    wasm_stack value = *stack;
    Float_kind kind = f32_kind(value->entry.value.value.u32);
    if(kind == Float_normal){
        if((value->entry.value.value.f32 != (i32_t)value->entry.value.value.f32) && (((value->entry.value.value.u32 & 0x7f800000) >> 23) < 150)){
            i32_t ival = value->entry.value.value.i32 & 0x80000000;
            value->entry.value.value.f32 = (i32_t)value->entry.value.value.f32;
            value->entry.value.value.i32 = ival | (value->entry.value.value.i32 & 0x7fffffff);
        }
    }else if(kind & 1){
        value->entry.value.value.u32 |= 0x7fc00000;
    }
    label->entry.label.current += 1;
}
void exec_f32_nearest(wasm_stack label, wasm_stack* stack){
    wasm_stack value = *stack;
    Float_kind kind = f32_kind(value->entry.value.value.u32);
    if((kind == Float_normal) && (value->entry.value.value.i32 & 0x7fffffffU)){
        u8_t expo = (value->entry.value.value.u32 & 0x7f800000U) >> 23;
        if(expo < 150){
            u32_t abs_value = value->entry.value.value.u32 & 0x7fffffffU;
            if(abs_value < 0x3f800000U){
                value->entry.value.value.u32 &= 0x80000000U;
                if(abs_value > 0x3f000000U){
                    value->entry.value.value.u32 |= 0x3f800000U;
                }
            }else{
                u32_t mantissa = value->entry.value.value.u32 & 0x7fffffU;
                u32_t mask = 0x7fffffU >> (expo - 127);
                u32_t round = (mask + 1) >> 1;
                value->entry.value.value.u32 += round;
                if((mantissa & mask) != round){
                    value->entry.value.value.u32 &= ~mask;
                }else{
                    value->entry.value.value.u32 &= ~(mask + (round << 1));
                }
            }
        }
    }else if(kind & 1){
        value->entry.value.value.u32 |= 0x7fc00000;
    }
    label->entry.label.current += 1;
}
void exec_f32_sqrt(wasm_stack label, wasm_stack* stack){
    // Reference: https://www.codeproject.com/Articles/69941/Best-Square-Root-Method-Algorithm-Function-Precisi

    wasm_stack value = *stack;
    Float_kind kind = f32_kind(value->entry.value.value.u32);
    if(kind & 1){
        value->entry.value.value.u32 |= 0x7fc00000;
    }else if((value->entry.value.value.u32 & 0x80000000U) && (value->entry.value.value.u32 != 0x80000000U)){
        // Negative
        value->entry.value.value.u32 = 0x7fc00000;
    }else if((kind == Float_normal) && (value->entry.value.value.u32 & 0x7fffffffU)){
        _Bool isDenorm = (value->entry.value.value.u32 & 0x7f800000U) == 0;
        if(isDenorm){
            value->entry.value.value.f32 *= 0x1000000U;
        }
        f32_t original = value->entry.value.value.f32;
        value->entry.value.value.u32 = 0x5f3759dfU - (value->entry.value.value.u32 >> 1); // Magic number from "Hacker's Delight"
        value->entry.value.value.f32 = original * value->entry.value.value.f32 * (1.5f - 0.5f * original * value->entry.value.value.f32 * value->entry.value.value.f32);
        // Two Babylonian Method
        value->entry.value.value.f32 = 0.5 * (value->entry.value.value.f32 + original / value->entry.value.value.f32);
        value->entry.value.value.f32 = 0.5 * (value->entry.value.value.f32 + original / value->entry.value.value.f32);
        if(isDenorm){
            value->entry.value.value.f32 /= 0x1000U;
        }
    }
    label->entry.label.current += 1;
}
void exec_f32_add(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    Float_kind kind1 = f32_kind(value1->entry.value.value.u32), kind2 = f32_kind(value2->entry.value.value.u32);
    if((kind1 & 1) || (kind2 & 1)){
        if(kind2 & 1){
            value1->entry.value.value = value2->entry.value.value;
        }
        value1->entry.value.value.u32 |= 0x7fc00000;
    }else if((kind1 == Float_normal) && (kind2 == Float_normal)){
        if(
            ((value1->entry.value.value.u32 & 0x7fffffff) == (value2->entry.value.value.u32 & 0x7fffffff))
            && ((value1->entry.value.value.u32 & 0x80000000) != (value2->entry.value.value.u32 & 0x80000000))
        ){
            value1->entry.value.value.u32 = 0;
        }else{
            value1->entry.value.value.f32 += value2->entry.value.value.f32;
        }
    }else{
        if(kind2 != Float_normal){
            if(kind1 == Float_normal){
                value1->entry.value.value = value2->entry.value.value;
            }else if(kind2 != kind1){
                value1->entry.value.value.u32 = 0x7fc00000;
            }
        }
    }
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_f32_sub(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    Float_kind kind1 = f32_kind(value1->entry.value.value.u32), kind2 = f32_kind(value2->entry.value.value.u32);
    if((kind1 & 1) || (kind2 & 1)){
        if(kind2 & 1){
            value1->entry.value.value = value2->entry.value.value;
        }
        value1->entry.value.value.u32 |= 0x7fc00000;
    }else{
        // sub(a, b) = add(a, neg(b))
        value2->entry.value.value.u32 ^= 0x80000000;
        if((kind1 == Float_normal) && (kind2 == Float_normal)){
            if(
                ((value1->entry.value.value.u32 & 0x7fffffff) == (value2->entry.value.value.u32 & 0x7fffffff))
                && ((value1->entry.value.value.u32 & 0x80000000) != (value2->entry.value.value.u32 & 0x80000000))
            ){
                value1->entry.value.value.u32 = 0;
            }else{
                value1->entry.value.value.f32 += value2->entry.value.value.f32;
            }
        }else{
            if(kind2 != Float_normal){
                if(kind1 == Float_normal){
                    value1->entry.value.value = value2->entry.value.value;
                }else if(kind2 == kind1){
                    value1->entry.value.value.u32 = 0x7fc00000;
                }
            }
        }
    }
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_f32_mul(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    Float_kind kind1 = f32_kind(value1->entry.value.value.u32), kind2 = f32_kind(value2->entry.value.value.u32);
    if((kind1 & 1) || (kind2 & 1)){
        if(kind2 & 1){
            value1->entry.value.value = value2->entry.value.value;
        }
        value1->entry.value.value.u32 |= 0x7fc00000;
    }else if((kind1 != Float_normal) || (kind2 != Float_normal)){
        if(((value1->entry.value.value.u32 & 0x7fffffff) == 0) || ((value2->entry.value.value.u32 & 0x7fffffff) == 0)){
            value1->entry.value.value.u32 = 0x7fc00000;
        }else{
            value1->entry.value.value.u32 ^= value2->entry.value.value.u32 & 0x80000000;
            value1->entry.value.value.u32 &= 0xff800000;
            value1->entry.value.value.u32 |= 0x7f800000;
        }
    }else if(((value1->entry.value.value.u32 & 0x7fffffff) == 0) && ((value2->entry.value.value.u32 & 0x7fffffff) == 0)){
        value1->entry.value.value.u32 ^= value2->entry.value.value.u32 & 0x80000000;
    }else{
        value1->entry.value.value.f32 *= value2->entry.value.value.f32;
    }
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_f32_div(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    Float_kind kind1 = f32_kind(value1->entry.value.value.u32), kind2 = f32_kind(value2->entry.value.value.u32);
    if((kind1 & 1) || (kind2 & 1)){
        if(kind2 & 1){
            value1->entry.value.value = value2->entry.value.value;
        }
        value1->entry.value.value.u32 |= 0x7fc00000;
    }else if(((kind1 != Float_normal) && (kind2 != Float_normal)) || (((value1->entry.value.value.u32 & 0x7fffffff) == 0) && ((value2->entry.value.value.u32 & 0x7fffffff) == 0))){
        value1->entry.value.value.u32 = 0x7fc00000;
    }else if((kind1 != Float_normal) || ((value2->entry.value.value.u32 & 0x7fffffff) == 0)){
        value1->entry.value.value.u32 ^= value2->entry.value.value.u32 & 0x80000000;
        value1->entry.value.value.u32 &= 0xff800000;
        value1->entry.value.value.u32 |= 0x7f800000;
    }else if(((value1->entry.value.value.u32 & 0x7fffffff) == 0) || (kind2 != Float_normal)){
        value1->entry.value.value.u32 ^= value2->entry.value.value.u32 & 0x80000000;
        value1->entry.value.value.u32 &= 0x80000000;
    }else{
        value1->entry.value.value.f32 /= value2->entry.value.value.f32;
    }
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_f32_min(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    Float_kind kind1 = f32_kind(value1->entry.value.value.u32), kind2 = f32_kind(value2->entry.value.value.u32);
    if((kind1 & 1) || (kind2 & 1)){
        if(kind2 & 1){
            value1->entry.value.value = value2->entry.value.value;
        }
        value1->entry.value.value.u32 |= 0x7fc00000;
    }else if((kind1 == Float_neg_inf) || (kind2 == Float_neg_inf)){
        value1->entry.value.value.u32 = 0xff800000;
    }else if(((value1->entry.value.value.u32 & 0x7fffffff) == 0) && ((value2->entry.value.value.u32 & 0x7fffffff) == 0) && (value1->entry.value.value.u32 != value2->entry.value.value.u32)){
        value1->entry.value.value.u32 = 0x80000000;
    }else if((kind1 == Float_pos_inf) || (value2->entry.value.value.f32 <= value1->entry.value.value.f32)){
        value1->entry.value.value = value2->entry.value.value;
    }
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_f32_max(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    Float_kind kind1 = f32_kind(value1->entry.value.value.u32), kind2 = f32_kind(value2->entry.value.value.u32);
    if((kind1 & 1) || (kind2 & 1)){
        if(kind2 & 1){
            value1->entry.value.value = value2->entry.value.value;
        }
        value1->entry.value.value.u32 |= 0x7fc00000;
    }else if((kind1 == Float_pos_inf) || (kind2 == Float_pos_inf)){
        value1->entry.value.value.u32 = 0x7f800000;
    }else if(((value1->entry.value.value.u32 & 0x7fffffff) == 0) && ((value2->entry.value.value.u32 & 0x7fffffff) == 0) && (value1->entry.value.value.u32 != value2->entry.value.value.u32)){
        value1->entry.value.value.u32 = 0;
    }else if((kind2 != Float_neg_inf) && ((kind1 == Float_neg_inf) || (value2->entry.value.value.f32 >= value1->entry.value.value.f32))){
        value1->entry.value.value = value2->entry.value.value;
    }
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_f32_copysign(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    value1->entry.value.value.u32 = (value1->entry.value.value.u32 & 0x7fffffff) | (value2->entry.value.value.u32 & 0x80000000);
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_f64_abs(wasm_stack label, wasm_stack* stack){
    wasm_stack value = *stack;
    value->entry.value.value.u64 &= 0x7fffffffffffffffLLU;
    label->entry.label.current += 1;
}
void exec_f64_neg(wasm_stack label, wasm_stack* stack){
    wasm_stack value = *stack;
    value->entry.value.value.u64 ^= 0x8000000000000000LLU;
    label->entry.label.current += 1;
}
void exec_f64_ceil(wasm_stack label, wasm_stack* stack){
    wasm_stack value = *stack;
    Float_kind kind = f64_kind(value->entry.value.value.u64);
    if(kind == Float_normal){
        if((value->entry.value.value.f64 != (i64_t)value->entry.value.value.f64) && (((value->entry.value.value.u64 & 0x7ff0000000000000LLU) >> 52) < 1075)){
            if(value->entry.value.value.f64 < 0){
                value->entry.value.value.f64 = (i64_t)value->entry.value.value.f64;
                value->entry.value.value.u64 |= 0x8000000000000000LLU;
            }else{
                value->entry.value.value.f64 = (i64_t)value->entry.value.value.f64 + 1;
            }
        }
    }else if(kind & 1){
        value->entry.value.value.u64 |= 0x7ff8000000000000LLU;
    }
    label->entry.label.current += 1;
}
void exec_f64_floor(wasm_stack label, wasm_stack* stack){
    wasm_stack value = *stack;
    Float_kind kind = f64_kind(value->entry.value.value.u64);
    if(kind == Float_normal){
        if((value->entry.value.value.f64 != (i64_t)value->entry.value.value.f64) && (((value->entry.value.value.u64 & 0x7ff0000000000000LLU) >> 52) < 1075)){
            if(value->entry.value.value.f64 < 0){
                value->entry.value.value.f64 = (i64_t)value->entry.value.value.f64 - 1;
            }else{
                value->entry.value.value.f64 = (i64_t)value->entry.value.value.f64;
                value->entry.value.value.u64 &= 0x7fffffffffffffffLLU;
            }
        }
    }else if(kind & 1){
        value->entry.value.value.u64 |= 0x7ff8000000000000LLU;
    }
    label->entry.label.current += 1;
}
void exec_f64_trunc(wasm_stack label, wasm_stack* stack){
    wasm_stack value = *stack;
    Float_kind kind = f64_kind(value->entry.value.value.u64);
    if(kind == Float_normal){
        if((value->entry.value.value.f64 != (i64_t)value->entry.value.value.f64) && (((value->entry.value.value.u64 & 0x7ff0000000000000LLU) >> 52) < 1075)){
            i64_t ival = value->entry.value.value.i64 & 0x8000000000000000LLU;
            value->entry.value.value.f64 = (i64_t)value->entry.value.value.f64;
            value->entry.value.value.i64 = ival | (value->entry.value.value.i64 & 0x7fffffffffffffffLLU);
        }
    }else if(kind & 1){
        value->entry.value.value.u64 |= 0x7ff8000000000000LLU;
    }
    label->entry.label.current += 1;
}
void exec_f64_nearest(wasm_stack label, wasm_stack* stack){
    wasm_stack value = *stack;
    Float_kind kind = f64_kind(value->entry.value.value.u64);
    if((kind == Float_normal) && (value->entry.value.value.i64 & 0x7fffffffffffffffLLU)){
        u16_t expo = ((value->entry.value.value.u64 & 0x7ff0000000000000LLU) >> 52) & 0xfff;
        if(expo < 1075){
            u64_t abs_value = value->entry.value.value.u64 & 0x7fffffffffffffffLLU;
            if(abs_value < 0x3ff0000000000000LLU){
                value->entry.value.value.u64 &= 0x8000000000000000LLU;
                if(abs_value > 0x3fe0000000000000LLU){
                    value->entry.value.value.u64 |= 0x3ff0000000000000LLU;
                }
            }else{
                u64_t mantissa = value->entry.value.value.u64 & 0xfffffffffffffLLU;
                u64_t mask = 0xfffffffffffffLLU >> (expo - 1023);
                u64_t round = (mask + 1) >> 1;
                value->entry.value.value.u64 += round;
                if((mantissa & mask) != round){
                    value->entry.value.value.u64 &= ~mask;
                }else{
                    value->entry.value.value.u64 &= ~(mask + (round << 1));
                }
            }
        }
    }else if(kind & 1){
        value->entry.value.value.u64 |= 0x7ff8000000000000LLU;
    }
    label->entry.label.current += 1;
}
void exec_f64_sqrt(wasm_stack label, wasm_stack* stack){
    // Reference: https://www.codeproject.com/Articles/69941/Best-Square-Root-Method-Algorithm-Function-Precisi

    wasm_stack value = *stack;
    Float_kind kind = f64_kind(value->entry.value.value.u64);
    if(kind & 1){
        value->entry.value.value.u64 |= 0x7ff8000000000000LLU;
    }else if((value->entry.value.value.u64 & 0x8000000000000000LLU) && (value->entry.value.value.u64 != 0x8000000000000000LLU)){
        // Negative
        value->entry.value.value.u64 = 0x7ff8000000000000LLU;
    }else if((kind == Float_normal) && (value->entry.value.value.u64 & 0x7fffffffffffffffLLU)){
        _Bool isDenorm = (value->entry.value.value.u64 & 0x7ff0000000000000LLU) == 0;
        if(isDenorm){
            value->entry.value.value.f64 *= 0x10000000000000LLU;
        }
        f64_t original = value->entry.value.value.f64;
        value->entry.value.value.u64 = 0x5fe6eb50c7b537a9LLU - (value->entry.value.value.u64 >> 1); // Magic number from "Hacker's Delight"
        value->entry.value.value.f64 = original * value->entry.value.value.f64 * (1.5f - 0.5f * original * value->entry.value.value.f64 * value->entry.value.value.f64);
        // Three Babylonian Method
        value->entry.value.value.f64 = 0.5 * (value->entry.value.value.f64 + original / value->entry.value.value.f64);
        value->entry.value.value.f64 = 0.5 * (value->entry.value.value.f64 + original / value->entry.value.value.f64);
        value->entry.value.value.f64 = 0.5 * (value->entry.value.value.f64 + original / value->entry.value.value.f64);
        if(isDenorm){
            value->entry.value.value.f64 /= 0x4000000U;
        }
    }
    label->entry.label.current += 1;
}
void exec_f64_add(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    Float_kind kind1 = f64_kind(value1->entry.value.value.u64), kind2 = f64_kind(value2->entry.value.value.u64);
    if((kind1 & 1) || (kind2 & 1)){
        if(kind2 & 1){
            value1->entry.value.value = value2->entry.value.value;
        }
        value1->entry.value.value.u64 |= 0x7ff8000000000000LLU;
    }else if((kind1 == Float_normal) && (kind2 == Float_normal)){
        if(
            ((value1->entry.value.value.u64 & 0x7fffffffffffffffLLU) == (value2->entry.value.value.u64 & 0x7fffffffffffffffLLU))
            && ((value1->entry.value.value.u64 & 0x8000000000000000LLU) != (value2->entry.value.value.u64 & 0x8000000000000000LLU))
        ){
            value1->entry.value.value.u64 = 0;
        }else{
            value1->entry.value.value.f64 += value2->entry.value.value.f64;
        }
    }else{
        if(kind2 != Float_normal){
            if(kind1 == Float_normal){
                value1->entry.value.value = value2->entry.value.value;
            }else if(kind2 != kind1){
                value1->entry.value.value.u64 = 0x7ff8000000000000LLU;
            }
        }
    }
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_f64_sub(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    Float_kind kind1 = f64_kind(value1->entry.value.value.u64), kind2 = f64_kind(value2->entry.value.value.u64);
    if((kind1 & 1) || (kind2 & 1)){
        if(kind2 & 1){
            value1->entry.value.value = value2->entry.value.value;
        }
        value1->entry.value.value.u64 |= 0x7ff8000000000000LLU;
    }else{
        // sub(a, b) = add(a, neg(b))
        value2->entry.value.value.u64 ^= 0x8000000000000000LLU;
        if((kind1 == Float_normal) && (kind2 == Float_normal)){
            if(
                ((value1->entry.value.value.u64 & 0x7fffffffffffffffLLU) == (value2->entry.value.value.u64 & 0x7fffffffffffffffLLU))
                && ((value1->entry.value.value.u64 & 0x8000000000000000LLU) != (value2->entry.value.value.u64 & 0x8000000000000000LLU))
            ){
                value1->entry.value.value.u64 = 0;
            }else{
                value1->entry.value.value.f64 += value2->entry.value.value.f64;
            }
        }else{
            if(kind2 != Float_normal){
                if(kind1 == Float_normal){
                    value1->entry.value.value = value2->entry.value.value;
                }else if(kind2 == kind1){
                    value1->entry.value.value.u64 = 0x7ff8000000000000LLU;
                }
            }
        }
    }
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_f64_mul(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    Float_kind kind1 = f64_kind(value1->entry.value.value.u64), kind2 = f64_kind(value2->entry.value.value.u64);
    if((kind1 & 1) || (kind2 & 1)){
        if(kind2 & 1){
            value1->entry.value.value = value2->entry.value.value;
        }
        value1->entry.value.value.u64 |= 0x7ff8000000000000LLU;
    }else if((kind1 != Float_normal) || (kind2 != Float_normal)){
        if(((value1->entry.value.value.u64 & 0x7fffffffffffffffLLU) == 0) || ((value2->entry.value.value.u64 & 0x7fffffffffffffffLLU) == 0)){
            value1->entry.value.value.u64 = 0x7ff8000000000000LLU;
        }else{
            value1->entry.value.value.u64 ^= value2->entry.value.value.u64 & 0x8000000000000000LLU;
            value1->entry.value.value.u64 &= 0xfff0000000000000LLU;
            value1->entry.value.value.u64 |= 0x7ff0000000000000LLU;
        }
    }else if(((value1->entry.value.value.u64 & 0x7fffffffffffffffLLU) == 0) && ((value2->entry.value.value.u64 & 0x7fffffffffffffffLLU) == 0)){
        value1->entry.value.value.u64 ^= value2->entry.value.value.u64 & 0x8000000000000000LLU;
    }else{
        value1->entry.value.value.f64 *= value2->entry.value.value.f64;
    }
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_f64_div(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    Float_kind kind1 = f64_kind(value1->entry.value.value.u64), kind2 = f64_kind(value2->entry.value.value.u64);
    if((kind1 & 1) || (kind2 & 1)){
        if(kind2 & 1){
            value1->entry.value.value = value2->entry.value.value;
        }
        value1->entry.value.value.u64 |= 0x7ff8000000000000LLU;
    }else if(((kind1 != Float_normal) && (kind2 != Float_normal)) || (((value1->entry.value.value.u64 & 0x7fffffffffffffffLLU) == 0) && ((value2->entry.value.value.u64 & 0x7fffffffffffffffLLU) == 0))){
        value1->entry.value.value.u64 = 0x7ff8000000000000LLU;
    }else if((kind1 != Float_normal) || ((value2->entry.value.value.u64 & 0x7fffffffffffffffLLU) == 0)){
        value1->entry.value.value.u64 ^= value2->entry.value.value.u64 & 0x8000000000000000LLU;
        value1->entry.value.value.u64 &= 0xfff0000000000000LLU;
        value1->entry.value.value.u64 |= 0x7ff0000000000000LLU;
    }else if(((value1->entry.value.value.u64 & 0x7fffffffffffffffLLU) == 0) || (kind2 != Float_normal)){
        value1->entry.value.value.u64 ^= value2->entry.value.value.u64 & 0x8000000000000000LLU;
        value1->entry.value.value.u64 &= 0x8000000000000000LLU;
    }else{
        value1->entry.value.value.f64 /= value2->entry.value.value.f64;
    }
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_f64_min(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    Float_kind kind1 = f64_kind(value1->entry.value.value.u64), kind2 = f64_kind(value2->entry.value.value.u64);
    if((kind1 & 1) || (kind2 & 1)){
        if(kind2 & 1){
            value1->entry.value.value = value2->entry.value.value;
        }
        value1->entry.value.value.u64 |= 0x7ff8000000000000LLU;
    }else if((kind1 == Float_neg_inf) || (kind2 == Float_neg_inf)){
        value1->entry.value.value.u64 = 0xfff0000000000000LLU;
    }else if(((value1->entry.value.value.u64 & 0x7fffffffffffffffLLU) == 0) && ((value2->entry.value.value.u64 & 0x7fffffffffffffffLLU) == 0) && (value1->entry.value.value.u64 != value2->entry.value.value.u64)){
        value1->entry.value.value.u64 = 0x8000000000000000LLU;
    }else if((kind1 == Float_pos_inf) || (value2->entry.value.value.f64 <= value1->entry.value.value.f64)){
        value1->entry.value.value = value2->entry.value.value;
    }
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_f64_max(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    Float_kind kind1 = f64_kind(value1->entry.value.value.u64), kind2 = f64_kind(value2->entry.value.value.u64);
    if((kind1 & 1) || (kind2 & 1)){
        if(kind2 & 1){
            value1->entry.value.value = value2->entry.value.value;
        }
        value1->entry.value.value.u64 |= 0x7ff8000000000000LLU;
    }else if((kind1 == Float_pos_inf) || (kind2 == Float_pos_inf)){
        value1->entry.value.value.u64 = 0x7ff0000000000000LLU;
    }else if(((value1->entry.value.value.u64 & 0x7fffffffffffffffLLU) == 0) && ((value2->entry.value.value.u64 & 0x7fffffffffffffffLLU) == 0) && (value1->entry.value.value.u64 != value2->entry.value.value.u64)){
        value1->entry.value.value.u64 = 0;
    }else if((kind1 == Float_neg_inf) || (value2->entry.value.value.f64 >= value1->entry.value.value.f64)){
        value1->entry.value.value = value2->entry.value.value;
    }
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_f64_copysign(wasm_stack label, wasm_stack* stack){
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    value1->entry.value.value.u64 = (value1->entry.value.value.u64 & 0x7fffffffffffffffLLU) | (value2->entry.value.value.u64 & 0x8000000000000000LLU);
    *stack = value1;
    free_func(value2);
    label->entry.label.current += 1;
}
void exec_i32_wrap_i64(wasm_stack label, wasm_stack* stack){
    wasm_stack value = *stack;
    value->entry.value.value.u64 &= 0x00000000ffffffffLLU;
    value->entry.value.type = Value_i32;
    label->entry.label.current += 1;
}
void exec_i32_trunc_s_f32(wasm_stack label, wasm_stack* stack){
    wasm_stack value = *stack;
    switch(f32_kind(value->entry.value.value.u32)){
        case Float_normal:
            if((value->entry.value.value.f32 >= 2147483648.0F) || (value->entry.value.value.f32 < -2147483648.0F)){
                wasmvm_errno = ERROR_int_overflow;
                return;
            }
            value->entry.value.value.i32 = (i32_t)value->entry.value.value.f32;
            value->entry.value.type = Value_i32;
        break;
        case Float_neg_inf:
        case Float_pos_inf:
            wasmvm_errno = ERROR_int_overflow;
            return;
        case Float_neg_nan:
        case Float_pos_nan:
            wasmvm_errno = ERROR_invalid_conv_int;
            return;
    }
    label->entry.label.current += 1;
}
void exec_i32_trunc_u_f32(wasm_stack label, wasm_stack* stack){
    wasm_stack value = *stack;
    switch(f32_kind(value->entry.value.value.u32)){
        case Float_normal:
            if((value->entry.value.value.f32 >= 4294967296.0F) || (value->entry.value.value.f32 == -1.0F)){
                wasmvm_errno = ERROR_int_overflow;
                return;
            }
            value->entry.value.value.u32 = (u32_t)value->entry.value.value.f32;
            value->entry.value.type = Value_i32;
        break;
        case Float_neg_inf:
        case Float_pos_inf:
            wasmvm_errno = ERROR_int_overflow;
            return;
        case Float_neg_nan:
        case Float_pos_nan:
            wasmvm_errno = ERROR_invalid_conv_int;
            return;
    }
    label->entry.label.current += 1;
}
void exec_i32_trunc_s_f64(wasm_stack label, wasm_stack* stack){
    wasm_stack value = *stack;
    switch(f64_kind(value->entry.value.value.u64)){
        case Float_normal:
            if((value->entry.value.value.f64 >= 2147483648.0) || (value->entry.value.value.f64 <= -2147483649.0)){
                wasmvm_errno = ERROR_int_overflow;
                return;
            }
            value->entry.value.value.i32 = (i32_t)value->entry.value.value.f64;
            value->entry.value.type = Value_i32;
        break;
        case Float_neg_inf:
        case Float_pos_inf:
            wasmvm_errno = ERROR_int_overflow;
            return;
        case Float_neg_nan:
        case Float_pos_nan:
            wasmvm_errno = ERROR_invalid_conv_int;
            return;
    }
    label->entry.label.current += 1;
}
void exec_i32_trunc_u_f64(wasm_stack label, wasm_stack* stack){
    wasm_stack value = *stack;
    switch(f64_kind(value->entry.value.value.u64)){
        case Float_normal:
            if((value->entry.value.value.f64 >= 4294967296.0) || (value->entry.value.value.f64 == -1.0)){
                wasmvm_errno = ERROR_int_overflow;
                return;
            }
            value->entry.value.value.u32 = (u32_t)value->entry.value.value.f64;
            value->entry.value.type = Value_i32;
            break;
        case Float_neg_inf:
        case Float_pos_inf:
            wasmvm_errno = ERROR_int_overflow;
            return;
        case Float_neg_nan:
        case Float_pos_nan:
            wasmvm_errno = ERROR_invalid_conv_int;
            return;
    }
    label->entry.label.current += 1;
}
void exec_i64_extend_s_i32(wasm_stack label, wasm_stack* stack){
    wasm_stack value = *stack;
    if(value->entry.value.value.u32 & 0x80000000U){
        value->entry.value.value.u64 |= 0xffffffff00000000LLU;
    }else{
        value->entry.value.value.u64 &= 0x00000000ffffffffLLU;
    }
    value->entry.value.type = Value_i64;
    label->entry.label.current += 1;
}
void exec_i64_extend_u_i32(wasm_stack label, wasm_stack* stack){
    wasm_stack value = *stack;
    value->entry.value.value.u64 &= 0x00000000ffffffffLLU;
    value->entry.value.type = Value_i64;
    label->entry.label.current += 1;
}
void exec_i64_trunc_s_f32(wasm_stack label, wasm_stack* stack){
    wasm_stack value = *stack;
    switch(f32_kind(value->entry.value.value.u32)){
        case Float_normal:
            if((value->entry.value.value.f32 >= 9223372036854775808.0F) || (value->entry.value.value.f32 < -9223372036854775808.0F)){
                wasmvm_errno = ERROR_int_overflow;
                return;
            }
            value->entry.value.value.i64 = (i64_t)value->entry.value.value.f32;
            value->entry.value.type = Value_i64;
        break;
        case Float_neg_inf:
        case Float_pos_inf:
            wasmvm_errno = ERROR_int_overflow;
            return;
        case Float_neg_nan:
        case Float_pos_nan:
            wasmvm_errno = ERROR_invalid_conv_int;
            return;
    }
    label->entry.label.current += 1;
}
void exec_i64_trunc_u_f32(wasm_stack label, wasm_stack* stack){
    wasm_stack value = *stack;
    switch(f32_kind(value->entry.value.value.u32)){
        case Float_normal:
            if((value->entry.value.value.f32 >= 18446744073709551616.0F) || (value->entry.value.value.f32 == -1.0F)){
                wasmvm_errno = ERROR_int_overflow;
                return;
            }
            value->entry.value.value.u64 = (u64_t)value->entry.value.value.f32;
            value->entry.value.type = Value_i64;
        break;
        case Float_neg_inf:
        case Float_pos_inf:
            wasmvm_errno = ERROR_int_overflow;
            return;
        case Float_neg_nan:
        case Float_pos_nan:
            wasmvm_errno = ERROR_invalid_conv_int;
            return;
    }
    label->entry.label.current += 1;
}
void exec_i64_trunc_s_f64(wasm_stack label, wasm_stack* stack){
    wasm_stack value = *stack;
    switch(f64_kind(value->entry.value.value.u64)){
        case Float_normal:
            if((value->entry.value.value.f64 >= 9223372036854775808.0) || (value->entry.value.value.f64 < -9223372036854775808.0)){
                wasmvm_errno = ERROR_int_overflow;
                return;
            }
            value->entry.value.value.i64 = (i64_t)value->entry.value.value.f64;
            value->entry.value.type = Value_i64;
        break;
        case Float_neg_inf:
        case Float_pos_inf:
            wasmvm_errno = ERROR_int_overflow;
            return;
        case Float_neg_nan:
        case Float_pos_nan:
            wasmvm_errno = ERROR_invalid_conv_int;
            return;
    }
    label->entry.label.current += 1;
}
void exec_i64_trunc_u_f64(wasm_stack label, wasm_stack* stack){
    wasm_stack value = *stack;
    switch(f64_kind(value->entry.value.value.u64)){
        case Float_normal:
            if((value->entry.value.value.f64 >= 18446744073709551616.0) || (value->entry.value.value.f64 == -1.0)){
                wasmvm_errno = ERROR_int_overflow;
                return;
            }
            value->entry.value.value.u64 = (u64_t)value->entry.value.value.f64;
            value->entry.value.type = Value_i64;
        break;
        case Float_neg_inf:
        case Float_pos_inf:
            wasmvm_errno = ERROR_int_overflow;
            return;
        case Float_neg_nan:
        case Float_pos_nan:
            wasmvm_errno = ERROR_invalid_conv_int;
            return;
    }
    label->entry.label.current += 1;
}
void exec_f32_convert_s_i32(wasm_stack label, wasm_stack* stack){
    wasm_stack value = *stack;
    value->entry.value.value.f32 = (f32_t)value->entry.value.value.i32;
    value->entry.value.type = Value_f32;
    label->entry.label.current += 1;
}
void exec_f32_convert_u_i32(wasm_stack label, wasm_stack* stack){
    wasm_stack value = *stack;
    value->entry.value.value.f32 = (f32_t)value->entry.value.value.u32;
    value->entry.value.type = Value_f32;
    label->entry.label.current += 1;
}
void exec_f32_convert_s_i64(wasm_stack label, wasm_stack* stack){
    wasm_stack value = *stack;
    value->entry.value.value.f32 = (f32_t)value->entry.value.value.i64;
    value->entry.value.type = Value_f32;
    label->entry.label.current += 1;
}
void exec_f32_convert_u_i64(wasm_stack label, wasm_stack* stack){
    wasm_stack value = *stack;
    value->entry.value.value.f32 = (f32_t)value->entry.value.value.u64;
    value->entry.value.type = Value_f32;
    label->entry.label.current += 1;
}
void exec_f32_demote_f64(wasm_stack label, wasm_stack* stack){
    wasm_stack value = *stack;
    Float_kind kind = f64_kind(value->entry.value.value.u64);
    if(kind & 1){
        value->entry.value.value.u32 = 0x7fc00000 + ((value->entry.value.value.u64 & 0x7fffffffffffffffLLU) != 0x7ff8000000000000LLU);
    }else if(kind == Float_normal){
        if((value->entry.value.value.u64 & 0x7fffffffffffffffLLU) != 0){
            value->entry.value.value.f32 = (f32_t)value->entry.value.value.f64;
        }else{
            value->entry.value.value.u64 >>= 32;
        }
    }else{
        value->entry.value.value.u64 >>= 32;
        value->entry.value.value.u32 &= 0x80000000;
        value->entry.value.value.u32 |= 0x7f800000;
    }
    value->entry.value.type = Value_f32;
    label->entry.label.current += 1;
}
void exec_f64_convert_s_i32(wasm_stack label, wasm_stack* stack){
    wasm_stack value = *stack;
    value->entry.value.value.f64 = (f64_t)value->entry.value.value.i32;
    value->entry.value.type = Value_f64;
    label->entry.label.current += 1;
}
void exec_f64_convert_u_i32(wasm_stack label, wasm_stack* stack){
    wasm_stack value = *stack;
    value->entry.value.value.f64 = (f64_t)value->entry.value.value.u32;
    value->entry.value.type = Value_f64;
    label->entry.label.current += 1;
}
void exec_f64_convert_s_i64(wasm_stack label, wasm_stack* stack){
    wasm_stack value = *stack;
    value->entry.value.value.f64 = (f64_t)value->entry.value.value.i64;
    value->entry.value.type = Value_f64;
    label->entry.label.current += 1;
}
void exec_f64_convert_u_i64(wasm_stack label, wasm_stack* stack){
    wasm_stack value = *stack;
    value->entry.value.value.f64 = (f64_t)value->entry.value.value.u64;
    value->entry.value.type = Value_f64;
    label->entry.label.current += 1;
}
void exec_f64_promote_f32(wasm_stack label, wasm_stack* stack){
    wasm_stack value = *stack;
    Float_kind kind = f32_kind(value->entry.value.value.u32);
    value->entry.value.value.u64 &= 0x00000000ffffffff;
    if(kind & 1){
        value->entry.value.value.u32 &= 0x3fffff;
        value->entry.value.value.u64 |= 0x7ff8000000000000LLU;
    }else if(kind == Float_normal){
        if((value->entry.value.value.u32 & 0x7fffffffU) != 0){
            value->entry.value.value.f64 = (f64_t)value->entry.value.value.f32;
        }else{
            if(value->entry.value.value.u32 & 0x80000000U){
                value->entry.value.value.u64 = 0x8000000000000000LLU;
            }else{
                value->entry.value.value.u64 = 0;
            }
        }
    }else{
        value->entry.value.value.u64 <<= 32;
        value->entry.value.value.u64 &= 0x8000000000000000LLU;
        value->entry.value.value.u64 |= 0x7ff0000000000000LLU;
    }
    value->entry.value.type = Value_f64;
    label->entry.label.current += 1;
}
void exec_i32_reinterpret_f32(wasm_stack label, wasm_stack* stack){
    wasm_stack value = *stack;
    value->entry.value.type = Value_i32;
    label->entry.label.current += 1;
}
void exec_i64_reinterpret_f64(wasm_stack label, wasm_stack* stack){
    wasm_stack value = *stack;
    value->entry.value.type = Value_i64;
    label->entry.label.current += 1;
}
void exec_f32_reinterpret_i32(wasm_stack label, wasm_stack* stack){
    wasm_stack value = *stack;
    value->entry.value.type = Value_f32;
    label->entry.label.current += 1;
}
void exec_f64_reinterpret_i64(wasm_stack label, wasm_stack* stack){
    wasm_stack value = *stack;
    value->entry.value.type = Value_f64;
    label->entry.label.current += 1;
}
void exec_i32_extend8_s(wasm_stack label, wasm_stack* stack){
    wasm_stack value = *stack;
    value->entry.value.value.i32 = (i32_t)(i8_t)value->entry.value.value.i32;
    label->entry.label.current += 1;
}
void exec_i32_extend16_s(wasm_stack label, wasm_stack* stack){
    wasm_stack value = *stack;
    value->entry.value.value.i32 = (i32_t)(i16_t)value->entry.value.value.i32;
    label->entry.label.current += 1;
}
void exec_i64_extend8_s(wasm_stack label, wasm_stack* stack){
    wasm_stack value = *stack;
    value->entry.value.value.i64 = (i64_t)(i8_t)value->entry.value.value.i64;
    label->entry.label.current += 1;
}
void exec_i64_extend16_s(wasm_stack label, wasm_stack* stack){
    wasm_stack value = *stack;
    value->entry.value.value.i64 = (i64_t)(i16_t)value->entry.value.value.i64;
    label->entry.label.current += 1;
}
void exec_i64_extend32_s(wasm_stack label, wasm_stack* stack){
    wasm_stack value = *stack;
    value->entry.value.value.i64 = (i64_t)(i32_t)value->entry.value.value.i64;
    label->entry.label.current += 1;
}
void exec_ref_null(wasm_stack* label, wasm_stack* frame, wasm_stack* stack, wasm_store store){
    // TODO:
}
void exec_ref_is_null(wasm_stack* label, wasm_stack* frame, wasm_stack* stack, wasm_store store){
    // TODO:
}
void exec_ref_func(wasm_stack* label, wasm_stack* frame, wasm_stack* stack, wasm_store store){
    // TODO:
}
void exec_i32_trunc_sat_f32_s(wasm_stack label, wasm_stack* stack){
    wasm_stack value = *stack;
    Float_kind kind = f32_kind(value->entry.value.value.u32);
    u8_t expo = (value->entry.value.value.u32 & 0x7f800000U) >> 23;
    if((kind & 1) || (expo == 0)){
        value->entry.value.value.u32 = 0;
    }else if((kind == Float_normal) && !((expo < 95) || (expo > 159))){
        if(value->entry.value.value.f32 < 0x80000000){
            value->entry.value.value.i32 = (i32_t)value->entry.value.value.f32;
        }else{
            value->entry.value.value.i32 = 0x7fffffff;
        }
    }else if((value->entry.value.value.u32 & 0x7fffffff) != 0){
        value->entry.value.value.u32 = 0x7fffffff + ((value->entry.value.value.u32 & 0x80000000) != 0);
    }
    value->entry.value.type = Value_i32;
    label->entry.label.current += 1;
}
void exec_i32_trunc_sat_f32_u(wasm_stack label, wasm_stack* stack){
    wasm_stack value = *stack;
    Float_kind kind = f32_kind(value->entry.value.value.u32);
    u8_t expo = (value->entry.value.value.u32 & 0x7f800000U) >> 23;
    if((kind & 1) || (value->entry.value.value.u32 & 0x80000000) || (expo == 0)){
        value->entry.value.value.u32 = 0;
    }else if((kind == Float_pos_inf) || (expo > 159)){
        value->entry.value.value.u32 = 0xffffffff;
    }else{
        if(value->entry.value.value.f32 < 0x100000000){
            value->entry.value.value.u32 = (u32_t)value->entry.value.value.f32;
        }else{
            value->entry.value.value.u32 = 0xffffffff;
        }
    }
    value->entry.value.type = Value_i32;
    label->entry.label.current += 1;
}
void exec_i32_trunc_sat_f64_s(wasm_stack label, wasm_stack* stack){
    wasm_stack value = *stack;
    Float_kind kind = f64_kind(value->entry.value.value.u64);
    u16_t expo = (value->entry.value.value.u64 & 0x7ff0000000000000LLU) >> 52;
    if((kind & 1) || (expo == 0)){
        value->entry.value.value.u32 = 0;
    }else if((kind == Float_normal) && !((expo < 991) || (expo > 1055))){
        if(value->entry.value.value.f64 < 0x80000000){
            value->entry.value.value.i32 = (i32_t)value->entry.value.value.f64;
        }else{
            value->entry.value.value.i32 = 0x7fffffff;
        }
    }else if((value->entry.value.value.u64 & 0x7fffffffffffffffLLU) != 0){
        value->entry.value.value.u32 = 0x7fffffff + ((value->entry.value.value.u64 & 0x8000000000000000LLU) != 0);
    }
    value->entry.value.type = Value_i32;
    label->entry.label.current += 1;
}
void exec_i32_trunc_sat_f64_u(wasm_stack label, wasm_stack* stack){
    wasm_stack value = *stack;
    Float_kind kind = f64_kind(value->entry.value.value.u64);
    u16_t expo = (value->entry.value.value.u64 & 0x7ff0000000000000LLU) >> 52;
    if((kind & 1) || (value->entry.value.value.u64 & 0x8000000000000000LLU) || (expo == 0)){
        value->entry.value.value.u32 = 0;
    }else if((kind == Float_pos_inf) || (expo > 1055)){
        value->entry.value.value.u32 = 0xffffffff;
    }else{
        if(value->entry.value.value.f64 < 0xffffffff){
            value->entry.value.value.u32 = (u32_t)value->entry.value.value.f64;
        }else{
            value->entry.value.value.u32 = 0xffffffff;
        }
    }
    value->entry.value.type = Value_i32;
    label->entry.label.current += 1;
}
void exec_i64_trunc_sat_f32_s(wasm_stack label, wasm_stack* stack){
    wasm_stack value = *stack;
    Float_kind kind = f32_kind(value->entry.value.value.u32);
    u8_t expo = (value->entry.value.value.u32 & 0x7f800000U) >> 23;
    if((kind & 1) || (expo == 0)){
        value->entry.value.value.u64 = 0;
    }else if((kind == Float_normal) && !((expo < 63) || (expo > 191))){
        if(value->entry.value.value.f32 < 0x8000000000000000LLU){
            value->entry.value.value.i64 = (i64_t)value->entry.value.value.f32;
        }else{
            value->entry.value.value.i64 = 0x7fffffffffffffffLLU;
        }
    }else if((value->entry.value.value.u32 & 0x7fffffff) != 0){
        value->entry.value.value.u64 = 0x7fffffffffffffffLLU + ((value->entry.value.value.u32 & 0x80000000) != 0);
    }
    value->entry.value.type = Value_i64;
    label->entry.label.current += 1;
}
void exec_i64_trunc_sat_f32_u(wasm_stack label, wasm_stack* stack){
    wasm_stack value = *stack;
    Float_kind kind = f32_kind(value->entry.value.value.u32);
    u8_t expo = (value->entry.value.value.u32 & 0x7f800000U) >> 23;
    if((kind & 1) || (value->entry.value.value.u32 & 0x80000000) || (expo == 0)){
        value->entry.value.value.u64 = 0;
    }else if((kind == Float_pos_inf) || (expo > 191)){
        value->entry.value.value.u64 = 0xffffffffffffffffLLU;
    }else{
        if(value->entry.value.value.f32 < (f32_t)0xffffffffffffffffLLU){
            value->entry.value.value.u64 = (u64_t)value->entry.value.value.f32;
        }else{
            value->entry.value.value.u64 = 0xffffffffffffffffLLU;
        }
    }
    value->entry.value.type = Value_i64;
    label->entry.label.current += 1;
}
void exec_i64_trunc_sat_f64_s(wasm_stack label, wasm_stack* stack){
    wasm_stack value = *stack;
    Float_kind kind = f64_kind(value->entry.value.value.u64);
    u16_t expo = (value->entry.value.value.u64 & 0x7ff0000000000000LLU) >> 52;
    if((kind & 1) || (expo == 0)){
        value->entry.value.value.u64 = 0;
    }else if((kind == Float_normal) && !((expo < 959) || (expo > 1087))){
        if(value->entry.value.value.f64 < 0x8000000000000000LLU){
            value->entry.value.value.i64 = (i64_t)value->entry.value.value.f64;
        }else{
            value->entry.value.value.i64 = 0x7fffffffffffffffLLU;
        }
    }else if((value->entry.value.value.u64 & 0x7fffffffffffffffLLU) != 0){
        value->entry.value.value.u64 = 0x7fffffffffffffffLLU + ((value->entry.value.value.u64 & 0x8000000000000000LLU) != 0);
    }
    value->entry.value.type = Value_i64;
    label->entry.label.current += 1;
}
void exec_i64_trunc_sat_f64_u(wasm_stack label, wasm_stack* stack){
    wasm_stack value = *stack;
    Float_kind kind = f64_kind(value->entry.value.value.u64);
    u16_t expo = (value->entry.value.value.u64 & 0x7ff0000000000000LLU) >> 52;
    if((kind & 1) || (value->entry.value.value.u64 & 0x8000000000000000LLU) || (expo == 0)){
        value->entry.value.value.u64 = 0;
    }else if((kind == Float_pos_inf) || (expo > 1087)){
        value->entry.value.value.u64 = 0xffffffffffffffffLLU;
    }else{
        if(value->entry.value.value.f64 < (f64_t)0xffffffffffffffffLLU){
            value->entry.value.value.u64 = (u64_t)value->entry.value.value.f64;
        }else{
            value->entry.value.value.u64 = 0xffffffffffffffffLLU;
        }
    }
    value->entry.value.type = Value_i64;
    label->entry.label.current += 1;
}
void exec_memory_init(wasm_stack* label, wasm_stack* frame, wasm_stack* stack, wasm_store store){
    // TODO:
}
void exec_data_drop(wasm_stack* label, wasm_stack* frame, wasm_stack* stack, wasm_store store){
    // TODO:
}
void exec_memory_copy(wasm_stack* label, wasm_stack* frame, wasm_stack* stack, wasm_store store){
    // TODO:
}
void exec_memory_fill(wasm_stack* label, wasm_stack* frame, wasm_stack* stack, wasm_store store){
    // TODO:
}
void exec_table_init(wasm_stack* label, wasm_stack* frame, wasm_stack* stack, wasm_store store){
    // TODO:
}
void exec_elem_drop(wasm_stack* label, wasm_stack* frame, wasm_stack* stack, wasm_store store){
    // TODO:
}
void exec_table_copy(wasm_stack* label, wasm_stack* frame, wasm_stack* stack, wasm_store store){
    // TODO:
}
void exec_table_grow(wasm_stack* label, wasm_stack* frame, wasm_stack* stack, wasm_store store){
    // TODO:
}
void exec_table_size(wasm_stack* label, wasm_stack* frame, wasm_stack* stack, wasm_store store){
    // TODO:
}
void exec_table_fill(wasm_stack* label, wasm_stack* frame, wasm_stack* stack, wasm_store store){
    // TODO:
}