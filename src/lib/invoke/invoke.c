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
            frame->entry.frame.locals.data[funcInst->type->params.size - 1 - i] = entry->entry.value;
            *stack = entry->next;
            free_func(entry);
        }
        // Initialize default values to frame locals
        for(u32_t i = 0; i < funcInst->body.wasm.locals.size; ++i){
            Value* value = frame->entry.frame.locals.data + (funcInst->type->params.size + i);
            value->type = funcInst->body.wasm.locals.data[i];
            value->value.i64 = 0;
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
            case Op_unreachable:
                exec_unreachable(current_label);
            break;
            case Op_nop:
                exec_nop(current_label);
            break;
            case Op_block:
                exec_block(&current_label, current_frame, stack);
            break;
            case Op_if:
                exec_if(&current_label, current_frame, stack);
            break;
            case Op_else:
                exec_else(current_label);
            break;
            case Op_end:
                exec_end(&current_label, &current_frame, stack);
            break;
            case Op_br:
                exec_br(&current_label, &current_frame, stack);
            break;
            case Op_br_if:
                exec_br_if(&current_label, &current_frame, stack);
            break;
            case Op_br_table:
                exec_br_table(&current_label, &current_frame, stack);
            break;
            case Op_local_get:
                exec_local_get(current_label, current_frame, stack, store);
            break;
            case Op_local_set:
                exec_local_set(current_label, current_frame, stack, store);
            break;
            case Op_local_tee:
                exec_local_tee(current_label, current_frame, stack, store);
            break;
            case Op_global_get:
                exec_global_get(current_label, current_frame, stack, store);
            break;
            case Op_global_set:
                exec_global_set(current_label, current_frame, stack, store);
            break;
            case Op_i32_load:
                exec_i32_load(current_label, current_frame, stack, store);
            break;
            case Op_i64_load:
                exec_i64_load(current_label, current_frame, stack, store);
            break;
            case Op_f32_load:
                exec_f32_load(current_label, current_frame, stack, store);
            break;
            case Op_f64_load:
                exec_f64_load(current_label, current_frame, stack, store);
            break;
            case Op_i32_load8_s:
                exec_i32_load8_s(current_label, current_frame, stack, store);
            break;
            case Op_i32_load8_u:
                exec_i32_load8_u(current_label, current_frame, stack, store);
            break;
            case Op_i32_load16_s:
                exec_i32_load16_s(current_label, current_frame, stack, store);
            break;
            case Op_i32_load16_u:
                exec_i32_load16_u(current_label, current_frame, stack, store);
            break;
            case Op_i64_load8_s:
                exec_i64_load8_s(current_label, current_frame, stack, store);
            break;
            case Op_i64_load8_u:
                exec_i64_load8_u(current_label, current_frame, stack, store);
            break;
            case Op_i64_load16_s:
                exec_i64_load16_s(current_label, current_frame, stack, store);
            break;
            case Op_i64_load16_u:
                exec_i64_load16_u(current_label, current_frame, stack, store);
            break;
            case Op_i64_load32_s:
                exec_i64_load32_s(current_label, current_frame, stack, store);
            break;
            case Op_i64_load32_u:
                exec_i64_load32_u(current_label, current_frame, stack, store);
            break;
            case Op_i32_store:
                exec_i32_store(current_label, current_frame, stack, store);
            break;
            case Op_i64_store:
                exec_i64_store(current_label, current_frame, stack, store);
            break;
            case Op_f32_store:
                exec_f32_store(current_label, current_frame, stack, store);
            break;
            case Op_f64_store:
                exec_f64_store(current_label, current_frame, stack, store);
            break;
            case Op_i32_store8:
                exec_i32_store8(current_label, current_frame, stack, store);
            break;
            case Op_i32_store16:
                exec_i32_store16(current_label, current_frame, stack, store);
            break;
            case Op_i64_store8:
                exec_i64_store8(current_label, current_frame, stack, store);
            break;
            case Op_i64_store16:
                exec_i64_store16(current_label, current_frame, stack, store);
            break;
            case Op_i64_store32:
                exec_i64_store32(current_label, current_frame, stack, store);
            break;
            case Op_memory_size:
                exec_memory_size(current_label, current_frame, stack, store);
            break;
            case Op_memory_grow:
                exec_memory_grow(current_label, current_frame, stack, store);
            break;
            case Op_i32_const:
            case Op_i64_const:
            case Op_f32_const:
            case Op_f64_const:
                exec_const(current_label, stack);
            break;
            case Op_i32_eqz:
                exec_i32_eqz(current_label, stack);
            break;
            case Op_i32_eq:
                exec_i32_eq(current_label, stack);
            break;
            case Op_i32_ne:
                exec_i32_ne(current_label, stack);
            break;
            case Op_i32_lt_s:
                exec_i32_lt_s(current_label, stack);
            break;
            case Op_i32_lt_u:
                exec_i32_lt_u(current_label, stack);
            break;
            case Op_i32_gt_s:
                exec_i32_gt_s(current_label, stack);
            break;
            case Op_i32_gt_u:
                exec_i32_gt_u(current_label, stack);
            break;
            case Op_i32_le_s:
                exec_i32_le_s(current_label, stack);
            break;
            case Op_i32_le_u:
                exec_i32_le_u(current_label, stack);
            break;
            case Op_i32_ge_s:
                exec_i32_ge_s(current_label, stack);
            break;
            case Op_i32_ge_u:
                exec_i32_ge_u(current_label, stack);
            break;
            case Op_i64_eqz:
                exec_i64_eqz(current_label, stack);
            break;
            case Op_i64_eq:
                exec_i64_eq(current_label, stack);
            break;
            case Op_i64_ne:
                exec_i64_ne(current_label, stack);
            break;
            case Op_i64_lt_s:
                exec_i64_lt_s(current_label, stack);
            break;
            case Op_i64_lt_u:
                exec_i64_lt_u(current_label, stack);
            break;
            case Op_i64_gt_s:
                exec_i64_gt_s(current_label, stack);
            break;
            case Op_i64_gt_u:
                exec_i64_gt_u(current_label, stack);
            break;
            case Op_i64_le_s:
                exec_i64_le_s(current_label, stack);
            break;
            case Op_i64_le_u:
                exec_i64_le_u(current_label, stack);
            break;
            case Op_i64_ge_s:
                exec_i64_ge_s(current_label, stack);
            break;
            case Op_i64_ge_u:
                exec_i64_ge_u(current_label, stack);
            break;
            case Op_f32_eq:
                exec_f32_eq(current_label, stack);
            break;
            case Op_f32_ne:
                exec_f32_ne(current_label, stack);
            break;
            case Op_f32_lt:
                exec_f32_lt(current_label, stack);
            break;
            case Op_f32_le:
                exec_f32_le(current_label, stack);
            break;
            case Op_f32_gt:
                exec_f32_gt(current_label, stack);
            break;
            case Op_f32_ge:
                exec_f32_ge(current_label, stack);
            break;
            case Op_f64_eq:
                exec_f64_eq(current_label, stack);
            break;
            case Op_f64_ne:
                exec_f64_ne(current_label, stack);
            break;
            case Op_f64_lt:
                exec_f64_lt(current_label, stack);
            break;
            case Op_f64_le:
                exec_f64_le(current_label, stack);
            break;
            case Op_f64_gt:
                exec_f64_gt(current_label, stack);
            break;
            case Op_f64_ge:
                exec_f64_ge(current_label, stack);
            break;
            case Op_i32_clz:
                exec_i32_clz(current_label, stack);
            break;
            case Op_i32_ctz:
                exec_i32_ctz(current_label, stack);
            break;
            case Op_i32_popcnt:
                exec_i32_popcnt(current_label, stack);
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
            case Op_i32_rem_s:
                exec_i32_rem_s(current_label, stack);
            break;
            case Op_i32_rem_u:
                exec_i32_rem_u(current_label, stack);
            break;
            case Op_i32_and:
                exec_i32_and(current_label, stack);
            break;
            case Op_i32_or:
                exec_i32_or(current_label, stack);
            break;
            case Op_i32_xor:
                exec_i32_xor(current_label, stack);
            break;
            case Op_i32_shl:
                exec_i32_shl(current_label, stack);
            break;
            case Op_i32_shr_s:
                exec_i32_shr_s(current_label, stack);
            break;
            case Op_i32_shr_u:
                exec_i32_shr_u(current_label, stack);
            break;
            case Op_i32_rotl:
                exec_i32_rotl(current_label, stack);
            break;
            case Op_i32_rotr:
                exec_i32_rotr(current_label, stack);
            break;
            case Op_i64_clz:
                exec_i64_clz(current_label, stack);
            break;
            case Op_i64_ctz:
                exec_i64_ctz(current_label, stack);
            break;
            case Op_i64_popcnt:
                exec_i64_popcnt(current_label, stack);
            break;
            case Op_i64_add:
                exec_i64_add(current_label, stack);
            break;
            case Op_i64_sub:
                exec_i64_sub(current_label, stack);
            break;
            case Op_i64_mul:
                exec_i64_mul(current_label, stack);
            break;
            case Op_i64_div_s:
                exec_i64_div_s(current_label, stack);
            break;
            case Op_i64_div_u:
                exec_i64_div_u(current_label, stack);
            break;
            case Op_i64_rem_s:
                exec_i64_rem_s(current_label, stack);
            break;
            case Op_i64_rem_u:
                exec_i64_rem_u(current_label, stack);
            break;
            case Op_i64_and:
                exec_i64_and(current_label, stack);
            break;
            case Op_i64_or:
                exec_i64_or(current_label, stack);
            break;
            case Op_i64_xor:
                exec_i64_xor(current_label, stack);
            break;
            case Op_i64_shl:
                exec_i64_shl(current_label, stack);
            break;
            case Op_i64_shr_s:
                exec_i64_shr_s(current_label, stack);
            break;
            case Op_i64_shr_u:
                exec_i64_shr_u(current_label, stack);
            break;
            case Op_i64_rotl:
                exec_i64_rotl(current_label, stack);
            break;
            case Op_i64_rotr:
                exec_i64_rotr(current_label, stack);
            break;
            case Op_f32_abs:
                exec_f32_abs(current_label, stack);
            break;
            case Op_f32_neg:
                exec_f32_neg(current_label, stack);
            break;
            case Op_f32_ceil:
                exec_f32_ceil(current_label, stack);
            break;
            case Op_f32_floor:
                exec_f32_floor(current_label, stack);
            break;
            case Op_f32_trunc:
                exec_f32_trunc(current_label, stack);
            break;
            case Op_f32_nearest:
                exec_f32_nearest(current_label, stack);
            break;
            case Op_f32_sqrt:
                exec_f32_sqrt(current_label, stack);
            break;
            case Op_f32_add:
                exec_f32_add(current_label, stack);
            break;
            case Op_f32_sub:
                exec_f32_sub(current_label, stack);
            break;
            case Op_f32_mul:
                exec_f32_mul(current_label, stack);
            break;
            case Op_f32_div:
                exec_f32_div(current_label, stack);
            break;
            case Op_f32_min:
                exec_f32_min(current_label, stack);
            break;
            case Op_f32_max:
                exec_f32_max(current_label, stack);
            break;
            case Op_f32_copysign:
                exec_f32_copysign(current_label, stack);
            break;
            case Op_f64_abs:
                exec_f64_abs(current_label, stack);
            break;
            case Op_f64_neg:
                exec_f64_neg(current_label, stack);
            break;
            case Op_f64_ceil:
                exec_f64_ceil(current_label, stack);
            break;
            case Op_f64_floor:
                exec_f64_floor(current_label, stack);
            break;
            case Op_f64_trunc:
                exec_f64_trunc(current_label, stack);
            break;
            case Op_f64_nearest:
                exec_f64_nearest(current_label, stack);
            break;
            case Op_f64_sqrt:
                exec_f64_sqrt(current_label, stack);
            break;
            case Op_f64_add:
                exec_f64_add(current_label, stack);
            break;
            case Op_f64_sub:
                exec_f64_sub(current_label, stack);
            break;
            case Op_f64_mul:
                exec_f64_mul(current_label, stack);
            break;
            case Op_f64_div:
                exec_f64_div(current_label, stack);
            break;
            case Op_f64_min:
                exec_f64_min(current_label, stack);
            break;
            case Op_f64_max:
                exec_f64_max(current_label, stack);
            break;
            case Op_f64_copysign:
                exec_f64_copysign(current_label, stack);
            break;
            case Op_i32_wrap_i64:
                exec_i32_wrap_i64(current_label, stack);
            break;
            case Op_i32_trunc_s_f32:
                exec_i32_trunc_s_f32(current_label, stack);
            break;
            case Op_i32_trunc_u_f32:
                exec_i32_trunc_u_f32(current_label, stack);
            break;
            case Op_i32_trunc_s_f64:
                exec_i32_trunc_s_f64(current_label, stack);
            break;
            case Op_i32_trunc_u_f64:
                exec_i32_trunc_u_f64(current_label, stack);
            break;
            case Op_i64_extend_s_i32:
                exec_i64_extend_s_i32(current_label, stack);
            break;
            case Op_i64_extend_u_i32:
                exec_i64_extend_u_i32(current_label, stack);
            break;
            case Op_i64_trunc_s_f32:
                exec_i64_trunc_s_f32(current_label, stack);
            break;
            case Op_i64_trunc_u_f32:
                exec_i64_trunc_u_f32(current_label, stack);
            break;
            case Op_i64_trunc_s_f64:
                exec_i64_trunc_s_f64(current_label, stack);
            break;
            case Op_i64_trunc_u_f64:
                exec_i64_trunc_u_f64(current_label, stack);
            break;
            case Op_f32_convert_s_i32:
                exec_f32_convert_s_i32(current_label, stack);
            break;
            case Op_f32_convert_u_i32:
                exec_f32_convert_u_i32(current_label, stack);
            break;
            case Op_f32_convert_s_i64:
                exec_f32_convert_s_i64(current_label, stack);
            break;
            case Op_f32_convert_u_i64:
                exec_f32_convert_u_i64(current_label, stack);
            break;
            case Op_f64_convert_s_i32:
                exec_f64_convert_s_i32(current_label, stack);
            break;
            case Op_f32_demote_f64:
                exec_f32_demote_f64(current_label, stack);
            break;
            case Op_f64_convert_u_i32:
                exec_f64_convert_u_i32(current_label, stack);
            break;
            case Op_f64_convert_s_i64:
                exec_f64_convert_s_i64(current_label, stack);
            break;
            case Op_f64_convert_u_i64:
                exec_f64_convert_u_i64(current_label, stack);
            break;
            case Op_f64_promote_f32:
                exec_f64_promote_f32(current_label, stack);
            break;
            case Op_i32_reinterpret_f32:
                exec_i32_reinterpret_f32(current_label, stack);
            break;
            case Op_i64_reinterpret_f64:
                exec_i64_reinterpret_f64(current_label, stack);
            break;
            case Op_f32_reinterpret_i32:
                exec_f32_reinterpret_i32(current_label, stack);
            break;
            case Op_f64_reinterpret_i64:
                exec_f64_reinterpret_i64(current_label, stack);
            break;
            case Op_i32_extend8_s:
                exec_i32_extend8_s(current_label, stack);
            break;
            case Op_i32_extend16_s:
                exec_i32_extend16_s(current_label, stack);
            break;
            case Op_i64_extend8_s:
                exec_i64_extend8_s(current_label, stack);
            break;
            case Op_i64_extend16_s:
                exec_i64_extend16_s(current_label, stack);
            break;
            case Op_i64_extend32_s:
                exec_i64_extend32_s(current_label, stack);
            break;

            case Op_i32_trunc_sat_f32_s:
                exec_i32_trunc_sat_f32_s(current_label, stack);
            break;
            case Op_i32_trunc_sat_f32_u:
                exec_i32_trunc_sat_f32_u(current_label, stack);
            break;
            case Op_i32_trunc_sat_f64_s:
                exec_i32_trunc_sat_f64_s(current_label, stack);
            break;
            case Op_i32_trunc_sat_f64_u:
                exec_i32_trunc_sat_f64_u(current_label, stack);
            break;
            case Op_i64_trunc_sat_f32_s:
                exec_i64_trunc_sat_f32_s(current_label, stack);
            break;
            case Op_i64_trunc_sat_f32_u:
                exec_i64_trunc_sat_f32_u(current_label, stack);
            break;
            case Op_i64_trunc_sat_f64_s:
                exec_i64_trunc_sat_f64_s(current_label, stack);
            break;
            case Op_i64_trunc_sat_f64_u:
                exec_i64_trunc_sat_f64_u(current_label, stack);
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