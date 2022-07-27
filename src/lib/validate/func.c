/**
 * Copyright 2022 Luis Hsu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include "validate.h"
#include "stack.h"
#include <Opcodes.h>

#define check_errno(Expr) \
    Expr; \
    if(wasmvm_errno != ERROR_success){ \
        free_value_stack(value_stack); \
        free_ctrl_stack(ctrl_stack); \
        return 0; \
    }

#define expect_check(T) check_errno(expect_val(value_stack, ctrl_stack, T))
#define return_clean() \
    free_value_stack(value_stack); \
    free_ctrl_stack(ctrl_stack); \
    return 0;

static _Bool is_num(ValueType type){
    switch (type){
    case Value_unspecified:
    case Value_i32:
    case Value_i64:
    case Value_f32:
    case Value_f64:
        return 1;
    default:
        return 0;
    }
}

static _Bool is_ref(ValueType type){
    switch (type){
    case Value_unspecified:
    case Value_funcref:
    case Value_externref:
        return 1;
    default:
        return 0;
    }
}

_Bool func_validate(WasmFunc* func, WasmModule* module, ValidateContext* context){
    // Validate type
    if(func->type >= module->types.size){
        wasmvm_errno = ERROR_unknown_type;
        return 0;
    }
    // Validate expr
    value_stack_t value_stack = (value_stack_t)malloc_func(sizeof(ValueStack));
    value_stack->size = 0;
    value_stack->data = NULL;
    ctrl_stack_t ctrl_stack = (ctrl_stack_t)malloc_func(sizeof(CtrlStack));
    ctrl_stack->size = 0;
    ctrl_stack->data = NULL;

    FuncType funcType = module->types.data[func->type];

    // Locals & params
    ValueType locals[funcType.params.size + func->locals.size];
    size_t local_size = funcType.params.size + func->locals.size;
    if(local_size > 0){
        memcpy_func(locals, funcType.params.data, sizeof(ValueType) * funcType.params.size);
        memcpy_func(locals + funcType.params.size, func->locals.data, sizeof(ValueType) * func->locals.size);
    }

    size_t paramSize = funcType.params.size;
    funcType.params.size = 0;
    push_ctrl(value_stack, ctrl_stack, Op_block, funcType);
    funcType.params.size = paramSize;

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
                        expect_check(type->params.data[i]);
                    }
                    push_ctrl(value_stack, ctrl_stack, Op_block, *type);
                }else if(instr->imm.values.value.type != Value_unspecified){
                    FuncType type;
                    vector_init(type.params);
                    type.results.size = 1;
                    type.results.data = (ValueType*) malloc_func(sizeof(ValueType));
                    type.results.data[0] = instr->imm.values.value.type;
                    push_ctrl(value_stack, ctrl_stack, Op_block, type);
                }else{
                    FuncType type;
                    vector_init(type.params);
                    vector_init(type.results);
                    push_ctrl(value_stack, ctrl_stack, Op_block, type);
                }
            break;
            case Op_loop:
                if(instr->imm.values.value.type == Value_index){
                    FuncType* type = module->types.data + instr->imm.values.index;
                    for(size_t i = 0; i < type->params.size; ++i){
                        expect_check(type->params.data[i]);
                    }
                    push_ctrl(value_stack, ctrl_stack, Op_block, *type);
                }else if(instr->imm.values.value.type != Value_unspecified){
                    FuncType type;
                    vector_init(type.params);
                    type.results.size = 1;
                    type.results.data = (ValueType*) malloc_func(sizeof(ValueType));
                    type.results.data[0] = instr->imm.values.value.type;
                    push_ctrl(value_stack, ctrl_stack, Op_loop, type);
                }else{
                    FuncType type;
                    vector_init(type.params);
                    vector_init(type.results);
                    push_ctrl(value_stack, ctrl_stack, Op_block, type);
                }
            break;
            case Op_if:
                expect_check(Value_i32);
                if(instr->imm.values.value.type == Value_index){
                    FuncType* type = module->types.data + instr->imm.values.index;
                    for(size_t i = 0; i < type->params.size; ++i){
                        expect_check(type->params.data[i]);
                    }
                    push_ctrl(value_stack, ctrl_stack, Op_if, *type);
                }else if(instr->imm.values.value.type != Value_unspecified){
                    FuncType type;
                    vector_init(type.params);
                    type.results.size = 1;
                    type.results.data = (ValueType*) malloc_func(sizeof(ValueType));
                    type.results.data[0] = instr->imm.values.value.type;
                    push_ctrl(value_stack, ctrl_stack, Op_if, type);
                }else{
                    FuncType type;
                    vector_init(type.params);
                    vector_init(type.results);
                    push_ctrl(value_stack, ctrl_stack, Op_if, type);
                }
                break;
            case Op_else:{
                ctrl_frame frame = pop_ctrl(value_stack, ctrl_stack);
                if(wasmvm_errno != ERROR_success){
                    return_clean();
                }
                if(frame.opcode != Op_if){
                    wasmvm_errno = ERROR_type_mis;
                    return_clean();
                }
                push_ctrl(value_stack, ctrl_stack, Op_else, frame.types);
            }break;
            case Op_end:{
                ctrl_frame frame = pop_ctrl(value_stack, ctrl_stack);
                if(wasmvm_errno != ERROR_success){
                    return_clean();
                }
                for(size_t i = 0; i < frame.types.results.size; ++i){
                    push_val(value_stack, frame.types.results.data[i]);
                }
            }break;
            case Op_br:{
                if(ctrl_stack->size <= instr->imm.values.index){
                    wasmvm_errno = ERROR_type_mis;
                    return_clean();
                }
                label_type_t label_type = label_types(ctrl_stack, &funcType, instr->imm.values.index);
                if(!peek_vals(value_stack, ctrl_stack, label_type)){
                    wasmvm_errno = ERROR_type_mis;
                    return_clean();
                }
                set_unreachable(value_stack, ctrl_stack);
            }break;
            case Op_br_if:{
                if(ctrl_stack->size <= instr->imm.values.index){
                    wasmvm_errno = ERROR_type_mis;
                    return_clean();
                }
                expect_check(Value_i32);
                label_type_t label_type = label_types(ctrl_stack, &funcType, instr->imm.values.index);
                if(!peek_vals(value_stack, ctrl_stack, label_type)){
                    wasmvm_errno = ERROR_type_mis;
                    return_clean();
                }
                set_unreachable(value_stack, ctrl_stack);
            }break;
            case Op_br_table:{
                expect_check(Value_i32);
                u32_t* indices = (u32_t*)instr->imm.vec.data;
                u32_t m = instr->imm.vec.size - 1;
                if(ctrl_stack->size <= indices[m]){
                    wasmvm_errno = ERROR_type_mis;
                    return_clean();
                }
                label_type_t defaultType = label_types(ctrl_stack, &funcType, indices[m]);
                for(size_t n = 0; n < m; ++n){
                    if(ctrl_stack->size <= indices[n]){
                        wasmvm_errno = ERROR_type_mis;
                        return_clean();
                    }
                    label_type_t nType = label_types(ctrl_stack, &funcType, indices[n]);
                    if(nType->size != defaultType->size){
                        wasmvm_errno = ERROR_type_mis;
                        return_clean();
                    }
                    if(!peek_vals(value_stack, ctrl_stack, nType)){
                        wasmvm_errno = ERROR_type_mis;
                        return_clean();
                    }
                }
                for(size_t i = defaultType->size; i > 0; --i){
                    expect_check(defaultType->data[i - 1]);
                }
                set_unreachable(value_stack, ctrl_stack);
            }break;
            case Op_return:
                for(size_t i = funcType.results.size; i > 0; --i){
                    expect_check(funcType.results.data[i - 1]);
                }
                set_unreachable(value_stack, ctrl_stack);
            break;
            case Op_call:{
                if(instr->imm.values.index >= (module->funcs.size + context->funcs.size)){
                    wasmvm_errno = ERROR_unknown_func;
                    return_clean();
                }
                FuncType* type = NULL;
                if(instr->imm.values.index >= context->funcs.size){
                    type = module->types.data + module->funcs.data[instr->imm.values.index - context->funcs.size].type;
                }else{
                    type = module->types.data + context->funcs.data[instr->imm.values.index];
                }
                for(size_t i = type->params.size; i > 0; --i){
                    expect_check(type->params.data[i - 1]);
                }
                for(size_t i = 0; i < type->results.size; ++i){
                    push_val(value_stack, type->results.data[i]);
                }
            }break;
            case Op_call_indirect:
                if(instr->imm.values.value.value.u32 >= (module->tables.size + context->tables.size)){
                    wasmvm_errno = ERROR_unknown_table;
                    return_clean();
                }
                if(instr->imm.values.value.value.u32 >= context->tables.size){
                    if(module->tables.data[instr->imm.values.value.value.u32 - context->tables.size].refType != Ref_func){
                        wasmvm_errno = ERROR_type_mis;
                        return_clean();
                    }
                }else if(context->tables.data[instr->imm.values.value.value.u32].refType != Ref_func){
                    wasmvm_errno = ERROR_type_mis;
                    return_clean();
                }
                if(instr->imm.values.index >= module->types.size){
                    wasmvm_errno = ERROR_unknown_type;
                    return_clean();
                }
                expect_check(Value_i32);
                FuncType* type = module->types.data + instr->imm.values.index;
                for(size_t i = type->params.size; i > 0; --i){
                    expect_check(type->params.data[i - 1]);
                }
                for(size_t i = 0; i < type->results.size; ++i){
                    push_val(value_stack, type->results.data[i]);
                }
                break;
            case Op_ref_null:
            case Op_ref_is_null:
            case Op_ref_func:
                // TODO:
                break;
            case Op_drop:
                pop_val(value_stack, ctrl_stack);
            break;
            case Op_select:{
                expect_check(Value_i32);
                ValueType op1 = pop_val(value_stack, ctrl_stack);
                ValueType op2 = pop_val(value_stack, ctrl_stack);
                if(!is_num(op1) || !is_num(op2)){
                    wasmvm_errno = ERROR_type_mis;
                    return_clean();
                }
                if((op1 != op2) && (op1 != Value_unspecified) && (op2 != Value_unspecified)){
                    wasmvm_errno = ERROR_type_mis;
                    return_clean();
                }
                push_val(value_stack, (op1 == Value_unspecified) ? op2 : op1);
            }break;
            case Op_select_t:{
                expect_check(Value_i32);
                ValueType op1 = pop_val(value_stack, ctrl_stack);
                ValueType op2 = pop_val(value_stack, ctrl_stack);
                if((op1 != op2) && (op1 != Value_unspecified) && (op2 != Value_unspecified)){
                    wasmvm_errno = ERROR_type_mis;
                    return_clean();
                }
                ValueType actual = (op1 == Value_unspecified) ? op2 : op1;
                if(actual != Value_unspecified){
                    _Bool checked = 0;
                    ValueType* types = (ValueType*)instr->imm.vec.data;
                    for(size_t i = 0; i < instr->imm.vec.size; ++i){
                        if(actual == types[i]){
                            checked = 1;
                        }
                    }
                    if(!checked){
                        wasmvm_errno = ERROR_type_mis;
                        return_clean();
                    }
                }
                push_val(value_stack, Value_unspecified);
            }break;
            case Op_local_get:
                if(instr->imm.values.index >= local_size){
                    wasmvm_errno = ERROR_len_out_of_bound;
                    return_clean();
                }
                push_val(value_stack, locals[instr->imm.values.index]);
            break;
            case Op_local_set:
                if(instr->imm.values.index >= local_size){
                    wasmvm_errno = ERROR_len_out_of_bound;
                    return_clean();
                }
                expect_check(locals[instr->imm.values.index]);
            break;
            case Op_local_tee:
                if(instr->imm.values.index >= local_size){
                    wasmvm_errno = ERROR_len_out_of_bound;
                    return_clean();
                }
                expect_check(locals[instr->imm.values.index]);
                push_val(value_stack, locals[instr->imm.values.index]);
            break;
            case Op_global_get:
                if(instr->imm.values.index >= module->globals.size){
                    wasmvm_errno = ERROR_len_out_of_bound;
                    return_clean();
                }
                push_val(value_stack, module->globals.data[instr->imm.values.index].valType);
            break;
            case Op_global_set:
                if(instr->imm.values.index >= module->globals.size){
                    wasmvm_errno = ERROR_len_out_of_bound;
                    return_clean();
                }
                if(!module->globals.data[instr->imm.values.index].mut){
                    wasmvm_errno = ERROR_immut_global;
                    return_clean();
                }
                expect_check(module->globals.data[instr->imm.values.index].valType);
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
            case Op_i32_load8_s:
            case Op_i32_load8_u:
                if(instr->imm.memarg.align > 0){
                    wasmvm_errno = ERROR_align_gt_natural;
                    return_clean();
                }
                expect_check(Value_i32);
                push_val(value_stack, Value_i32);
            break;
            case Op_i32_load16_s:
            case Op_i32_load16_u:
                if(instr->imm.memarg.align > 1){
                    wasmvm_errno = ERROR_align_gt_natural;
                    return_clean();
                }
                expect_check(Value_i32);
                push_val(value_stack, Value_i32);
            break;
            case Op_i32_load:
                if(instr->imm.memarg.align > 2){
                    wasmvm_errno = ERROR_align_gt_natural;
                    return_clean();
                }
                expect_check(Value_i32);
                push_val(value_stack, Value_i32);
            break;
            case Op_i64_load8_s:
            case Op_i64_load8_u:
                if(instr->imm.memarg.align > 0){
                    wasmvm_errno = ERROR_align_gt_natural;
                    return_clean();
                }
                expect_check(Value_i32);
                push_val(value_stack, Value_i64);
            break;
            case Op_i64_load16_s:
            case Op_i64_load16_u:
                if(instr->imm.memarg.align > 1){
                    wasmvm_errno = ERROR_align_gt_natural;
                    return_clean();
                }
                expect_check(Value_i32);
                push_val(value_stack, Value_i64);
            break;
            case Op_i64_load32_s:
            case Op_i64_load32_u:
                if(instr->imm.memarg.align > 2){
                    wasmvm_errno = ERROR_align_gt_natural;
                    return_clean();
                }
                expect_check(Value_i32);
                push_val(value_stack, Value_i64);
            break;
            case Op_i64_load:
                if(instr->imm.memarg.align > 3){
                    wasmvm_errno = ERROR_align_gt_natural;
                    return_clean();
                }
                expect_check(Value_i32);
                push_val(value_stack, Value_i64);
            break;
            case Op_f32_load:
                if(instr->imm.memarg.align > 2){
                    wasmvm_errno = ERROR_align_gt_natural;
                    return_clean();
                }
                expect_check(Value_i32);
                push_val(value_stack, Value_f32);
            break;
            case Op_f64_load:
                if(instr->imm.memarg.align > 3){
                    wasmvm_errno = ERROR_align_gt_natural;
                    return_clean();
                }
                expect_check(Value_i32);
                push_val(value_stack, Value_f64);
            break;
            case Op_i32_store8:
                if(instr->imm.memarg.align > 0){
                    wasmvm_errno = ERROR_align_gt_natural;
                    return_clean();
                }
                expect_check(Value_i32);
                expect_check(Value_i32);
            break;
            case Op_i32_store16:
                if(instr->imm.memarg.align > 1){
                    wasmvm_errno = ERROR_align_gt_natural;
                    return_clean();
                }
                expect_check(Value_i32);
                expect_check(Value_i32);
            break;
            case Op_i32_store:
                if(instr->imm.memarg.align > 2){
                    wasmvm_errno = ERROR_align_gt_natural;
                    return_clean();
                }
                expect_check(Value_i32);
                expect_check(Value_i32);
            break;
            case Op_i64_store:
                if(instr->imm.memarg.align > 3){
                    wasmvm_errno = ERROR_align_gt_natural;
                    return_clean();
                }
                expect_check(Value_i64);
                expect_check(Value_i32);
            break;
            case Op_i64_store8:
                if(instr->imm.memarg.align > 0){
                    wasmvm_errno = ERROR_align_gt_natural;
                    return_clean();
                }
                expect_check(Value_i64);
                expect_check(Value_i32);
            break;
            case Op_i64_store16:
                if(instr->imm.memarg.align > 1){
                    wasmvm_errno = ERROR_align_gt_natural;
                    return_clean();
                }
                expect_check(Value_i64);
                expect_check(Value_i32);
            break;
            case Op_i64_store32:
                if(instr->imm.memarg.align > 2){
                    wasmvm_errno = ERROR_align_gt_natural;
                    return_clean();
                }
                expect_check(Value_i64);
                expect_check(Value_i32);
            break;
            case Op_f32_store:
                if(instr->imm.memarg.align > 2){
                    wasmvm_errno = ERROR_align_gt_natural;
                    return_clean();
                }
                expect_check(Value_f32);
                expect_check(Value_i32);
            break;
            case Op_f64_store:
                if(instr->imm.memarg.align > 3){
                    wasmvm_errno = ERROR_align_gt_natural;
                    return_clean();
                }
                expect_check(Value_f64);
                expect_check(Value_i32);
            break;
            case Op_memory_size:
                if(module->mems.size <= 0){
                    wasmvm_errno = ERROR_unknown_mem;
                    return_clean();
                }
                push_val(value_stack, Value_i32);
            break;
            case Op_memory_grow:
                if(module->mems.size <= 0){
                    wasmvm_errno = ERROR_unknown_mem;
                    return_clean();
                }
                expect_check(Value_i32);
                push_val(value_stack, Value_i32);
            break;
            case Op_memory_init:
            case Op_data_drop:
            case Op_memory_copy:
            case Op_memory_fill:
                // TODO:
                break;
            case Op_i32_const:
                push_val(value_stack, Value_i32);
            break;
            case Op_i64_const:
                push_val(value_stack, Value_i64);
            break;
            case Op_f32_const:
                push_val(value_stack, Value_f32);
            break;
            case Op_f64_const:
                push_val(value_stack, Value_f64);
            break;
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
                expect_check(Value_i32);
                expect_check(Value_i32);
                push_val(value_stack, Value_i32);
            break;
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
                expect_check(Value_i64);
                expect_check(Value_i64);
                push_val(value_stack, Value_i32);
            break;
            case Op_f32_eq:
            case Op_f32_ne:
            case Op_f32_lt:
            case Op_f32_gt:
            case Op_f32_le:
            case Op_f32_ge:
                expect_check(Value_f32);
                expect_check(Value_f32);
                push_val(value_stack, Value_i32);
            break;
            case Op_f64_eq:
            case Op_f64_ne:
            case Op_f64_lt:
            case Op_f64_gt:
            case Op_f64_le:
            case Op_f64_ge:
                expect_check(Value_f64);
                expect_check(Value_f64);
                push_val(value_stack, Value_i32);
            break;
            case Op_i32_eqz:
            case Op_i32_clz:
            case Op_i32_ctz:
            case Op_i32_popcnt:
                expect_check(Value_i32);
                push_val(value_stack, Value_i32);
            break;
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
                expect_check(Value_i32);
                expect_check(Value_i32);
                push_val(value_stack, Value_i32);
            break;
            case Op_i64_eqz:
                expect_check(Value_i64);
                push_val(value_stack, Value_i32);
            break;
            case Op_i64_clz:
            case Op_i64_ctz:
            case Op_i64_popcnt:
                expect_check(Value_i64);
                push_val(value_stack, Value_i64);
            break;
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
                expect_check(Value_i64);
                expect_check(Value_i64);
                push_val(value_stack, Value_i64);
            break;
            case Op_f32_abs:
            case Op_f32_neg:
            case Op_f32_ceil:
            case Op_f32_floor:
            case Op_f32_trunc:
            case Op_f32_nearest:
            case Op_f32_sqrt:
                expect_check(Value_f32);
                push_val(value_stack, Value_f32);
            break;
            case Op_f32_add:
            case Op_f32_sub:
            case Op_f32_mul:
            case Op_f32_div:
            case Op_f32_min:
            case Op_f32_max:
            case Op_f32_copysign:
                expect_check(Value_f32);
                expect_check(Value_f32);
                push_val(value_stack, Value_f32);
            break;
            case Op_f64_abs:
            case Op_f64_neg:
            case Op_f64_ceil:
            case Op_f64_floor:
            case Op_f64_trunc:
            case Op_f64_nearest:
            case Op_f64_sqrt:
                expect_check(Value_f64);
                push_val(value_stack, Value_f64);
            break;
            case Op_f64_add:
            case Op_f64_sub:
            case Op_f64_mul:
            case Op_f64_div:
            case Op_f64_min:
            case Op_f64_max:
            case Op_f64_copysign:
                expect_check(Value_f64);
                expect_check(Value_f64);
                push_val(value_stack, Value_f64);
            break;
            case Op_i32_wrap_i64:
                expect_check(Value_i64);
                push_val(value_stack, Value_i32);
            break;
            case Op_i32_trunc_s_f32:
            case Op_i32_trunc_u_f32:
            case Op_i32_trunc_sat_f32_s:
            case Op_i32_trunc_sat_f32_u:
                expect_check(Value_f32);
                push_val(value_stack, Value_i32);
            break;
            case Op_i32_trunc_s_f64:
            case Op_i32_trunc_u_f64:
            case Op_i32_trunc_sat_f64_s:
            case Op_i32_trunc_sat_f64_u:
                expect_check(Value_f64);
                push_val(value_stack, Value_i32);
            break;
            case Op_i64_extend_s_i32:
            case Op_i64_extend_u_i32:
                expect_check(Value_i32);
                push_val(value_stack, Value_i64);
            break;
            case Op_i64_trunc_s_f32:
            case Op_i64_trunc_u_f32:
            case Op_i64_trunc_sat_f32_s:
            case Op_i64_trunc_sat_f32_u:
                expect_check(Value_f32);
                push_val(value_stack, Value_i64);
            break;
            case Op_i64_trunc_s_f64:
            case Op_i64_trunc_u_f64:
            case Op_i64_trunc_sat_f64_s:
            case Op_i64_trunc_sat_f64_u:
                expect_check(Value_f64);
                push_val(value_stack, Value_i64);
            break;
            case Op_f32_convert_s_i32:
            case Op_f32_convert_u_i32:
                expect_check(Value_i32);
                push_val(value_stack, Value_f32);
            break;
            case Op_f32_convert_s_i64:
            case Op_f32_convert_u_i64:
                expect_check(Value_i64);
                push_val(value_stack, Value_f32);
            break;
            case Op_f32_demote_f64:
                expect_check(Value_f64);
                push_val(value_stack, Value_f32);
            break;
            case Op_f64_convert_s_i32:
            case Op_f64_convert_u_i32:
                expect_check(Value_i32);
                push_val(value_stack, Value_f64);
            break;
            case Op_f64_convert_s_i64:
            case Op_f64_convert_u_i64:
                expect_check(Value_i64);
                push_val(value_stack, Value_f64);
            break;
            case Op_f64_promote_f32:
                expect_check(Value_f32);
                push_val(value_stack, Value_f64);
            break;
            case Op_i32_reinterpret_f32:
                expect_check(Value_f32);
                push_val(value_stack, Value_i32);
            break;
            case Op_i64_reinterpret_f64:
                expect_check(Value_f64);
                push_val(value_stack, Value_i64);
            break;
            case Op_f32_reinterpret_i32:
                expect_check(Value_i32);
                push_val(value_stack, Value_f32);
            break;
            case Op_f64_reinterpret_i64:
                expect_check(Value_i64);
                push_val(value_stack, Value_f64);
            break;
            case Op_i32_extend8_s:
            case Op_i32_extend16_s:
                expect_check(Value_i32);
                push_val(value_stack, Value_i32);
            break;
            case Op_i64_extend8_s:
            case Op_i64_extend16_s:
            case Op_i64_extend32_s:
                expect_check(Value_i64);
                push_val(value_stack, Value_i64);
            break;
            default:
                wasmvm_errno = ERROR_unknown_operator;
                return_clean();
        }
    }
    free_value_stack(value_stack);
    free_ctrl_stack(ctrl_stack);
    return 1;
}