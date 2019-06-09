#include "Allocates.h"

#include <stdlib.h>
#include <Opcodes.h>
#include <dataTypes/Value.h>
#include <structures/instrs/WasmInstr.h>
#include <structures/instrs/Control.h>
#include <structures/instrs/Parametric.h>
#include <structures/instrs/Variable.h>
#include <structures/instrs/Memory.h>
#include <structures/instrs/Numeric.h>
#include <instance/ControlInstrInst.h>
#include <instance/ParametricInstrInst.h>
#include <instance/VariableInstrInst.h>
#include <instance/MemoryInstrInst.h>
#include <instance/NumericInstrInst.h>

static InstrInst* allocate_ControlInstr(WasmControlInstr* instr, list_p funcBody, size_t index)
{
    ControlInstrInst* instrInst = new_ControlInstrInst();
    instrInst->parent.opcode = instr->parent.opcode;
    for(size_t i = 0; i < vector_size(instr->resultTypes); ++i) {
        ValueType* resultType = (ValueType*) malloc(sizeof(ValueType));
        *resultType = *vector_at(ValueType*, instr->resultTypes, i);
        vector_push_back(instrInst->resultTypes, resultType);
        free(resultType);
    }
    for(size_t i = 0; i < vector_size(instr->indices); ++i) {
        uint32_t* index = (uint32_t*) malloc(sizeof(uint32_t));
        *index = *vector_at(uint32_t*, instr->indices, i);
        vector_push_back(instrInst->indices, index);
        free(index);
    }
    if(instr->parent.opcode == Op_if || instr->parent.opcode == Op_block || instr->parent.opcode == Op_loop) {
        uint32_t endLevel = 0;
        for(size_t i = index + 1; i < list_size(funcBody); ++i) {
            InstrInst* curInstr = list_at(InstrInst*, funcBody, i);
            if(curInstr->opcode == Op_if || curInstr->opcode == Op_block || curInstr->opcode == Op_loop) {
                endLevel += 1;
            } else if(curInstr->opcode == Op_end) {
                if(endLevel == 0) {
                    instrInst->endAddr = i;
                    break;
                } else {
                    endLevel -= 1;
                }
            } else if(instr->parent.opcode == Op_if && curInstr->opcode == Op_else && endLevel == 0) {
                instrInst->elseAddr = i;
            }
        }
    }
    return (InstrInst*)instrInst;
}

static InstrInst* allocate_ParametricInstr(WasmParametricInstr* instr)
{
    ParametricInstrInst* instrInst = new_ParametricInstrInst();
    instrInst->opcode = instr->parent.opcode;
    return (InstrInst*)instrInst;
}

static InstrInst* allocate_VariableInstr(WasmVariableInstr* instr)
{
    VariableInstrInst* instrInst = new_VariableInstrInst();
    instrInst->parent.opcode = instr->parent.opcode;
    instrInst->index = instr->index;
    return (InstrInst*)instrInst;
}

static InstrInst* allocate_MemoryInstr(WasmMemoryInstr* instr)
{
    MemoryInstrInst* instrInst = new_MemoryInstrInst();
    instrInst->parent.opcode = instr->parent.opcode;
    instrInst->offset = instr->offset;
    instrInst->align = instr->align;
    return (InstrInst*)instrInst;
}

static InstrInst* allocate_NumericInstr(WasmNumericInstr* instr)
{
    NumericInstrInst* instrInst = new_NumericInstrInst();
    instrInst->parent.opcode = instr->parent.opcode;
    instrInst->constant = instr->constant;
    return (InstrInst*)instrInst;
}

InstrInst* allocate_Instruction(list_p funcBody, size_t index)
{
    WasmInstr* instr = list_at(WasmInstr*, funcBody, index);
    InstrInst* instrInst = NULL;
    switch (instr->opcode) {
        case Op_unreachable:
        case Op_nop:
        case Op_block:
        case Op_loop:
        case Op_if:
        case Op_else:
        case Op_end:
        case Op_br:
        case Op_br_if:
        case Op_br_table:
        case Op_return:
        case Op_call:
        case Op_call_indirect:
            instrInst = allocate_ControlInstr((WasmControlInstr*)instr, funcBody, index);
            break;
        case Op_drop:
        case Op_select:
            instrInst = allocate_ParametricInstr((WasmParametricInstr*)instr);
            break;
        case Op_get_local:
        case Op_set_local:
        case Op_tee_local:
        case Op_get_global:
        case Op_set_global:
            instrInst = allocate_VariableInstr((WasmVariableInstr*)instr);
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
            instrInst = allocate_MemoryInstr((WasmMemoryInstr*)instr);
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
            instrInst = allocate_NumericInstr((WasmNumericInstr*)instr);
            break;
        default:
            break;
    }
    return instrInst;
}