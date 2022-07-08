/**
 * Copyright 2022 Luis Hsu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include "validate.h"
#include "stack.h"
#include <Opcodes.h>

_Bool func_validate(WasmFunc* func, WasmModule* module){
    value_stack_t value_stack = (value_stack_t)malloc_func(sizeof(ValueStack));
    value_stack->size = 0;
    value_stack->data = NULL;
    ctrl_stack_t ctrl_stack = (ctrl_stack_t)malloc_func(sizeof(CtrlStack));
    ctrl_stack->size = 0;
    ctrl_stack->data = NULL;
    push_ctrl(value_stack, ctrl_stack, Op_block, module->types.data[func->type]);
    for(size_t instrIdx = 0; instrIdx < func->body.size; ++instrIdx){
        WasmInstr* instr = func->body.data + instrIdx;
        switch(instr->opcode){
            case Op_nop:
            break;
            case Op_unreachable:
                set_unreachable(value_stack, ctrl_stack);
            break;
            case Op_block:
                if(instr->imm.values.value.type == Value_index){
                    FuncType* type = module->types.data + instr->imm.values.index;
                    for(size_t i = 0; i < type->params.size; ++i){
                        expect_val(value_stack, ctrl_stack, type->params.data[i]);
                        if(wasmvm_errno != ERROR_success){
                            return 0;
                        }
                    }
                    push_ctrl(value_stack, ctrl_stack, Op_block, *type);
                }else if(instr->imm.values.value.type != Value_unspecified){
                    expect_val(value_stack, ctrl_stack, instr->imm.values.value.type);
                    if(wasmvm_errno != ERROR_success){
                        return 0;
                    }
                    FuncType type;
                    vector_init(type.params);
                    type.results.size = 1;
                    type.results.data = (ValueType*) malloc_func(sizeof(ValueType));
                    type.results.data[0] = instr->imm.values.value.type;
                    push_ctrl(value_stack, ctrl_stack, Op_block, type);
                }
            break;
            case Op_loop:
                if(instr->imm.values.value.type == Value_index){
                    FuncType* type = module->types.data + instr->imm.values.index;
                    for(size_t i = 0; i < type->params.size; ++i){
                        expect_val(value_stack, ctrl_stack, type->params.data[i]);
                        if(wasmvm_errno != ERROR_success){
                            return 0;
                        }
                    }
                    push_ctrl(value_stack, ctrl_stack, Op_block, *type);
                }else if(instr->imm.values.value.type != Value_unspecified){
                    expect_val(value_stack, ctrl_stack, instr->imm.values.value.type);
                    if(wasmvm_errno != ERROR_success){
                        return 0;
                    }
                    FuncType type;
                    vector_init(type.params);
                    type.results.size = 1;
                    type.results.data = (ValueType*) malloc_func(sizeof(ValueType));
                    type.results.data[0] = instr->imm.values.value.type;
                    push_ctrl(value_stack, ctrl_stack, Op_loop, type);
                }
            break;
            case Op_if:
                expect_val(value_stack, ctrl_stack, Value_i32);
                if(wasmvm_errno != ERROR_success){
                    return 0;
                }
                if(instr->imm.values.value.type == Value_index){
                    FuncType* type = module->types.data + instr->imm.values.index;
                    for(size_t i = 0; i < type->params.size; ++i){
                        expect_val(value_stack, ctrl_stack, type->params.data[i]);
                        if(wasmvm_errno != ERROR_success){
                            return 0;
                        }
                    }
                    push_ctrl(value_stack, ctrl_stack, Op_block, *type);
                }else if(instr->imm.values.value.type != Value_unspecified){
                    expect_val(value_stack, ctrl_stack, instr->imm.values.value.type);
                    if(wasmvm_errno != ERROR_success){
                        return 0;
                    }
                    FuncType type;
                    vector_init(type.params);
                    type.results.size = 1;
                    type.results.data = (ValueType*) malloc_func(sizeof(ValueType));
                    type.results.data[0] = instr->imm.values.value.type;
                    push_ctrl(value_stack, ctrl_stack, Op_if, type);
                }
            break;
            case Op_else:{
                ctrl_frame frame = pop_ctrl(value_stack, ctrl_stack);
                if(wasmvm_errno != ERROR_success){
                    return 0;
                }
                if(frame.opcode == Op_if){
                    wasmvm_errno = ERROR_type_mis;
                    return 0;
                }
                push_ctrl(value_stack, ctrl_stack, Op_else, frame.types);
            }break;
            case Op_end:{
                ctrl_frame frame = pop_ctrl(value_stack, ctrl_stack);
                if(wasmvm_errno != ERROR_success){
                    return 0;
                }
                for(size_t i = 0; i < frame.types.results.size; ++i){
                    push_val(value_stack, frame.types.results.data[i]);
                }
            }break;
            case Op_br:{
                if(ctrl_stack->size < instr->imm.values.index){
                    wasmvm_errno = ERROR_type_mis;
                    return 0;
                }
                label_type_t type = label_types(ctrl_stack, instr->imm.values.index);
                for(size_t i = 0; i < type->size; ++i){
                    expect_val(value_stack, ctrl_stack, type->data[i]);
                    if(wasmvm_errno != ERROR_success){
                        return 0;
                    }
                }
                set_unreachable(value_stack, ctrl_stack);
            }break;
            case Op_br_if:{
                if(ctrl_stack->size < instr->imm.values.index){
                    wasmvm_errno = ERROR_type_mis;
                    return 0;
                }
                expect_val(value_stack, ctrl_stack, Value_i32);
                if(wasmvm_errno != ERROR_success){
                    return 0;
                }
                label_type_t type = label_types(ctrl_stack, instr->imm.values.index);
                for(size_t i = 0; i < type->size; ++i){
                    expect_val(value_stack, ctrl_stack, type->data[i]);
                    if(wasmvm_errno != ERROR_success){
                        return 0;
                    }
                }
                set_unreachable(value_stack, ctrl_stack);
            }break;
            case Op_br_table:
            case Op_return:
            case Op_call:
            case Op_call_indirect:
                // TODO:
                break;
            case Op_ref_null:
            case Op_ref_is_null:
            case Op_ref_func:
                // TODO:
                break;
            case Op_drop:
            case Op_select:
            case Op_select_t:
                // TODO:
                break;
            case Op_local_get:
            case Op_local_set:
            case Op_local_tee:
            case Op_global_get:
            case Op_global_set:
                // TODO:
                break;
            case Op_table_set:
            case Op_table_get:
            case Op_table_init:
            case Op_elem_drop:
            case Op_table_copy:
            case Op_table_grow:
            case Op_table_size:
            case Op_table_fill:
                // TODO:
                break;
            case Op_i32_load:
            case Op_i64_load:
            case Op_f32_load:
            case Op_f64_load:
            case Op_i32_load8_s:
            case Op_i32_load8_u:
            case Op_i32_load16_s:
            case Op_i32_load16_u:
            case Op_i64_load8_s:
            case Op_i64_load8_u:
            case Op_i64_load16_s:
            case Op_i64_load16_u:
            case Op_i64_load32_s:
            case Op_i64_load32_u:
            case Op_i32_store:
            case Op_i64_store:
            case Op_f32_store:
            case Op_f64_store:
            case Op_i32_store8:
            case Op_i32_store16:
            case Op_i64_store8:
            case Op_i64_store16:
            case Op_i64_store32:
            case Op_memory_size:
            case Op_memory_grow:
            case Op_memory_init:
            case Op_data_drop:
            case Op_memory_copy:
            case Op_memory_fill:
                // TODO:
                break;
            case Op_i32_const:
            case Op_i64_const:
            case Op_f32_const:
            case Op_f64_const:
            case Op_i32_eqz:
            case Op_i32_eq:
            case Op_i32_ne:
            case Op_i32_lt_s:
            case Op_i32_lt_u:
            case Op_i32_gt_s:
            case Op_i32_gt_u:
            case Op_i32_le_s:
            case Op_i32_le_u:
            case Op_i32_ge_s:
            case Op_i32_ge_u:
            case Op_i64_eqz:
            case Op_i64_eq:
            case Op_i64_ne:
            case Op_i64_lt_s:
            case Op_i64_lt_u:
            case Op_i64_gt_s:
            case Op_i64_gt_u:
            case Op_i64_le_s:
            case Op_i64_le_u:
            case Op_i64_ge_s:
            case Op_i64_ge_u:
            case Op_f32_eq:
            case Op_f32_ne:
            case Op_f32_lt:
            case Op_f32_gt:
            case Op_f32_le:
            case Op_f32_ge:
            case Op_f64_eq:
            case Op_f64_ne:
            case Op_f64_lt:
            case Op_f64_gt:
            case Op_f64_le:
            case Op_f64_ge:
            case Op_i32_clz:
            case Op_i32_ctz:
            case Op_i32_popcnt:
            case Op_i32_add:
            case Op_i32_sub:
            case Op_i32_mul:
            case Op_i32_div_s:
            case Op_i32_div_u:
            case Op_i32_rem_s:
            case Op_i32_rem_u:
            case Op_i32_and:
            case Op_i32_or:
            case Op_i32_xor:
            case Op_i32_shl:
            case Op_i32_shr_s:
            case Op_i32_shr_u:
            case Op_i32_rotl:
            case Op_i32_rotr:
            case Op_i64_clz:
            case Op_i64_ctz:
            case Op_i64_popcnt:
            case Op_i64_add:
            case Op_i64_sub:
            case Op_i64_mul:
            case Op_i64_div_s:
            case Op_i64_div_u:
            case Op_i64_rem_s:
            case Op_i64_rem_u:
            case Op_i64_and:
            case Op_i64_or:
            case Op_i64_xor:
            case Op_i64_shl:
            case Op_i64_shr_s:
            case Op_i64_shr_u:
            case Op_i64_rotl:
            case Op_i64_rotr:
            case Op_f32_abs:
            case Op_f32_neg:
            case Op_f32_ceil:
            case Op_f32_floor:
            case Op_f32_trunc:
            case Op_f32_nearest:
            case Op_f32_sqrt:
            case Op_f32_add:
            case Op_f32_sub:
            case Op_f32_mul:
            case Op_f32_div:
            case Op_f32_min:
            case Op_f32_max:
            case Op_f32_copysign:
            case Op_f64_abs:
            case Op_f64_neg:
            case Op_f64_ceil:
            case Op_f64_floor:
            case Op_f64_trunc:
            case Op_f64_nearest:
            case Op_f64_sqrt:
            case Op_f64_add:
            case Op_f64_sub:
            case Op_f64_mul:
            case Op_f64_div:
            case Op_f64_min:
            case Op_f64_max:
            case Op_f64_copysign:
            case Op_i32_wrap_i64:
            case Op_i32_trunc_s_f32:
            case Op_i32_trunc_u_f32:
            case Op_i32_trunc_s_f64:
            case Op_i32_trunc_u_f64:
            case Op_i64_extend_s_i32:
            case Op_i64_extend_u_i32:
            case Op_i64_trunc_s_f32:
            case Op_i64_trunc_u_f32:
            case Op_i64_trunc_s_f64:
            case Op_i64_trunc_u_f64:
            case Op_f32_convert_s_i32:
            case Op_f32_convert_u_i32:
            case Op_f32_convert_s_i64:
            case Op_f32_convert_u_i64:
            case Op_f32_demote_f64:
            case Op_f64_convert_s_i32:
            case Op_f64_convert_u_i32:
            case Op_f64_convert_s_i64:
            case Op_f64_convert_u_i64:
            case Op_f64_promote_f32:
            case Op_i32_reinterpret_f32:
            case Op_i64_reinterpret_f64:
            case Op_f32_reinterpret_i32:
            case Op_f64_reinterpret_i64:
            case Op_i32_extend8_s:
            case Op_i32_extend16_s:
            case Op_i64_extend8_s:
            case Op_i64_extend16_s:
            case Op_i64_extend32_s:
            case Op_i32_trunc_sat_f32_s:
            case Op_i32_trunc_sat_f32_u:
            case Op_i32_trunc_sat_f64_s:
            case Op_i32_trunc_sat_f64_u:
            case Op_i64_trunc_sat_f32_s:
            case Op_i64_trunc_sat_f32_u:
            case Op_i64_trunc_sat_f64_s:
            case Op_i64_trunc_sat_f64_u:
                // TODO:
            break;
            default:
                wasmvm_errno = ERROR_unknown_operator;
                return 0;
        }
    }
    return 1;
}