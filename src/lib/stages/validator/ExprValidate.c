#include "Validates.h"

#include <stdlib.h>
#include <dataTypes/stack_p.h>
#include <Opcodes.h>

static void clean(stack_p opds, stack_p ctrls)
{
    free_stack_p(opds);
    free_stack_p(ctrls);
}

int validate_Expr(list_p expr, Context* context)
{
    // Prepare
    stack_p opds = new_stack_p(free); // ValueType
    stack_p ctrls = new_stack_p(free_ctrl_frame); // ctrl_frame
    ctrl_frame* frame = new_ctrl_frame(opds);
    stack_push(ctrls, frame);
    for(size_t i = 0; i < vector_size(context->returns); ++i) {
        vector_push_back(frame->end_types, vector_at(ValueType*, context->returns, i));
    }
    // Validate
    for(size_t i = 0; i < list_size(expr); ++i) {
        WasmInstr* instr = list_at(WasmInstr*, expr, i);
        int result = 0;
        switch (instr->opcode) {
            case Op_unreachable:
                result = validate_Instr_unreachable((WasmControlInstr*)instr, context, opds, ctrls);
                break;
            case Op_nop:
                result = validate_Instr_nop((WasmControlInstr*)instr, context, opds, ctrls);
                break;
            case Op_block:
                result = validate_Instr_block((WasmControlInstr*)instr, context, opds, ctrls);
                break;
            case Op_loop:
                result = validate_Instr_loop((WasmControlInstr*)instr, context, opds, ctrls);
                break;
            case Op_if:
                result = validate_Instr_if((WasmControlInstr*)instr, context, opds, ctrls);
                break;
            case Op_else:
                result = validate_Instr_else((WasmControlInstr*)instr, context, opds, ctrls);
                break;
            case Op_end:
                result = validate_Instr_end((WasmControlInstr*)instr, context, opds, ctrls);
                break;
            case Op_br:
                result = validate_Instr_br((WasmControlInstr*)instr, context, opds, ctrls);
                break;
            case Op_br_if:
                result = validate_Instr_br_if((WasmControlInstr*)instr, context, opds, ctrls);
                break;
            case Op_br_table:
                result = validate_Instr_br_table((WasmControlInstr*)instr, context, opds, ctrls);
                break;
            case Op_return:
                result = validate_Instr_return((WasmControlInstr*)instr, context, opds, ctrls);
                break;
            case Op_call:
                result = validate_Instr_call((WasmControlInstr*)instr, context, opds, ctrls);
                break;
            case Op_call_indirect:
                result = validate_Instr_call_indirect((WasmControlInstr*)instr, context, opds, ctrls);
                break;
            case Op_drop:
                result = validate_Instr_drop((WasmParametricInstr*)instr, context, opds, ctrls);
                break;
            case Op_select:
                result = validate_Instr_select((WasmParametricInstr*)instr, context, opds, ctrls);
                break;
            case Op_get_local:
                result = validate_Instr_get_local((WasmVariableInstr*)instr, context, opds, ctrls);
                break;
            case Op_set_local:
                result = validate_Instr_set_local((WasmVariableInstr*)instr, context, opds, ctrls);
                break;
            case Op_tee_local:
                result = validate_Instr_tee_local((WasmVariableInstr*)instr, context, opds, ctrls);
                break;
            case Op_get_global:
                result = validate_Instr_get_global((WasmVariableInstr*)instr, context, opds, ctrls);
                break;
            case Op_set_global:
                result = validate_Instr_set_global((WasmVariableInstr*)instr, context, opds, ctrls);
                break;
            case Op_i32_load:
            case Op_i64_load:
            case Op_f32_load:
            case Op_f64_load:
                result = validate_Instr_load((WasmMemoryInstr*)instr, context, opds, ctrls);
                break;
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
                result = validate_Instr_loadN((WasmMemoryInstr*)instr, context, opds, ctrls);
                break;
            case Op_i32_store:
            case Op_i64_store:
            case Op_f32_store:
            case Op_f64_store:
                result = validate_Instr_store((WasmMemoryInstr*)instr, context, opds, ctrls);
                break;
            case Op_i32_store8:
            case Op_i32_store16:
            case Op_i64_store8:
            case Op_i64_store16:
            case Op_i64_store32:
                result = validate_Instr_storeN((WasmMemoryInstr*)instr, context, opds, ctrls);
                break;
            case Op_memory_size:
                result = validate_Instr_memory_size((WasmMemoryInstr*)instr, context, opds, ctrls);
                break;
            case Op_memory_grow:
                result = validate_Instr_memory_grow((WasmMemoryInstr*)instr, context, opds, ctrls);
                break;
            case Op_i32_const:
            case Op_i64_const:
            case Op_f32_const:
            case Op_f64_const:
                result = validate_Instr_const((WasmNumericInstr*)instr, context, opds, ctrls);
                break;
            case Op_i32_eqz:
            case Op_i64_eqz:
                result = validate_Instr_testop((WasmNumericInstr*)instr, context, opds, ctrls);
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
                result = validate_Instr_relop((WasmNumericInstr*)instr, context, opds, ctrls);
                break;
            case Op_i32_clz:
            case Op_i32_ctz:
            case Op_i32_popcnt:
            case Op_i64_clz:
            case Op_i64_ctz:
            case Op_i64_popcnt:
            case Op_f32_abs:
            case Op_f32_neg:
            case Op_f32_ceil:
            case Op_f32_floor:
            case Op_f32_trunc:
            case Op_f32_nearest:
            case Op_f32_sqrt:
            case Op_f64_abs:
            case Op_f64_neg:
            case Op_f64_ceil:
            case Op_f64_floor:
            case Op_f64_trunc:
            case Op_f64_nearest:
            case Op_f64_sqrt:
                result = validate_Instr_unop((WasmNumericInstr*)instr, context, opds, ctrls);
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
            case Op_f32_add:
            case Op_f32_sub:
            case Op_f32_mul:
            case Op_f32_div:
            case Op_f32_min:
            case Op_f32_max:
            case Op_f32_copysign:
            case Op_f64_add:
            case Op_f64_sub:
            case Op_f64_mul:
            case Op_f64_div:
            case Op_f64_min:
            case Op_f64_max:
            case Op_f64_copysign:
                result = validate_Instr_binop((WasmNumericInstr*)instr, context, opds, ctrls);
                break;
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
                result = validate_Instr_cvtop((WasmNumericInstr*)instr, context, opds, ctrls);
                break;
            default:
                clean(opds, ctrls);
                return -1;
        }
        if(result) {
            clean(opds, ctrls);
            return result;
        }
    }
    // Check remaining operands
    if(stack_size(opds) != vector_size(context->returns)) {
        clean(opds, ctrls);
        return -2;
    }
    for(size_t i = 0; i < vector_size(context->returns); ++i) {
        ValueType* val = stack_pop(ValueType*, opds);
        ValueType* ret = vector_at(ValueType*, context->returns, i);
        if(*val != *ret) {
            clean(opds, ctrls);
            return -3;
        }
    }
    clean(opds, ctrls);
    return 0;
}
