/**
 * Copyright 2022 Luis Hsu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include "instrs.h"
#include <error.h>

size_t get_code_size(const WasmFunc* func){
    size_t code_size = 0;
    for(size_t instr_index = 0; instr_index < func->body.size; ++instr_index){
        const WasmInstr* instr = func->body.data + instr_index;
        switch (instr->opcode){
            // Constant instructions
            case Op_i32_const:
            case Op_i64_const:
            case Op_f32_const:
            case Op_f64_const:
                code_size += sizeof(ConstInstrInst);
                break;
            // Numeric instructions
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
                code_size += sizeof(InstrInst);
                break;
            // Reference instructions
            case Op_ref_null:
            case Op_ref_func:
                code_size += sizeof(UnaryInstrInst);
                break;
            case Op_ref_is_null:
                code_size += sizeof(InstrInst);
                break;
            // Parametric instructions
            case Op_drop:
            case Op_select:
                code_size += sizeof(InstrInst);
                break;
            case Op_select_t:
                code_size += sizeof(SelectInstrInst) + (instr->imm.vec.size * sizeof(ValueType));
                break;
            // Variable instructions
            case Op_local_get:
            case Op_local_set:
            case Op_local_tee:
            case Op_global_get:
            case Op_global_set:
                code_size += sizeof(UnaryInstrInst);
                break;
            // Table instructions
            case Op_table_get:
            case Op_table_set:
            case Op_elem_drop:
            case Op_table_grow:
            case Op_table_size:
            case Op_table_fill:
                code_size += sizeof(UnaryInstrInst);
                break;
            case Op_table_init:
            case Op_table_copy:
                code_size += sizeof(BinaryInstrInst);
                break;
            // Memory instructions
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
            case Op_memory_init:
            case Op_data_drop:
                code_size += sizeof(BinaryInstrInst);
                break;
            case Op_memory_size:
            case Op_memory_grow:
            case Op_memory_copy:
            case Op_memory_fill:
                code_size += sizeof(InstrInst);
                break;
            // Control instructions
            case Op_unreachable:
            case Op_nop:
            case Op_else:
            case Op_end:
            case Op_return:
                code_size += sizeof(InstrInst);
                break;
            case Op_block:
            case Op_loop:
            case Op_if:
                code_size += sizeof(BlockInstrInst);
                break;
            case Op_br:
            case Op_br_if:
            case Op_call:
                code_size += sizeof(UnaryInstrInst);
                break;
            case Op_call_indirect:
                code_size += sizeof(BinaryInstrInst);
                break;
            case Op_br_table:
                code_size += sizeof(BrTableInstrInst) + (instr->imm.vec.size * sizeof(u32_t));
                break;
            default:
                break;
        }
    }
    return code_size;
}

typedef struct _BlockNode{
    BlockInstrInst* ptr;
    u64_t offset;
    struct _BlockNode* next;
} BlockStack;

void fill_func_body(const WasmFunc* func, byte_t* data){
    const byte_t* const begin = data;
    BlockStack* blockStack = NULL;
    for(size_t instr_index = 0; instr_index < func->body.size; ++instr_index){
        const WasmInstr* instr = func->body.data + instr_index;
        ((InstrInst*)data)->opcode = instr->opcode;
        switch (instr->opcode){
            // Constant instructions
            case Op_i32_const:
            case Op_i64_const:
            case Op_f32_const:
            case Op_f64_const:
                ((ConstInstrInst*)data)->constant = instr->imm.values.value;
                data += sizeof(ConstInstrInst);
                break;
            // Numeric instructions
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
                data += sizeof(InstrInst);
                break;
            // Reference instructions
            case Op_ref_null:
            case Op_ref_func:
                ((UnaryInstrInst*)data)->index = instr->imm.values.index;
                data += sizeof(UnaryInstrInst);
                break;
            case Op_ref_is_null:
                data += sizeof(InstrInst);
                break;
            // Parametric instructions
            case Op_drop:
            case Op_select:
                data += sizeof(InstrInst);
                break;
            case Op_select_t:{
                SelectInstrInst* selectInstr = (SelectInstrInst*)data;
                selectInstr->size = instr->imm.vec.size;
                memcpy_func(selectInstr->params, instr->imm.vec.data, sizeof(ValueType) * selectInstr->size);
                data += sizeof(SelectInstrInst) + (instr->imm.vec.size * sizeof(ValueType));
                }break;
            // Variable instructions
            case Op_local_get:
            case Op_local_set:
            case Op_local_tee:
            case Op_global_get:
            case Op_global_set:
                ((UnaryInstrInst*)data)->index = instr->imm.values.index;
                data += sizeof(UnaryInstrInst);
                break;
            // Table instructions
            case Op_table_get:
            case Op_table_set:
            case Op_elem_drop:
            case Op_table_grow:
            case Op_table_size:
            case Op_table_fill:
                ((UnaryInstrInst*)data)->index = instr->imm.values.index;
                data += sizeof(UnaryInstrInst);
                break;
            case Op_table_init:
            case Op_table_copy:{
                BinaryInstrInst* binaryInstr = (BinaryInstrInst*)data;
                binaryInstr->index1 = instr->imm.values.index;
                binaryInstr->index2 = instr->imm.values.value.value.u32;
                data += sizeof(BinaryInstrInst);
                }break;
            // Memory instructions
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
            case Op_memory_init:
            case Op_data_drop:{
                BinaryInstrInst* binaryInstr = (BinaryInstrInst*)data;
                binaryInstr->index1 = instr->imm.memarg.align;
                binaryInstr->index2 = instr->imm.memarg.offset;
                data += sizeof(BinaryInstrInst);
                }break;
            case Op_memory_size:
            case Op_memory_grow:
            case Op_memory_copy:
            case Op_memory_fill:
                data += sizeof(InstrInst);
                break;
            // Control instructions
            case Op_else:
                if(blockStack->ptr->opcode != Op_if){
                    wasmvm_errno = ERROR_mis_label;
                    return;
                }
                blockStack->ptr->brOffset = data - begin - blockStack->offset;
                data += sizeof(InstrInst);
                break;
            case Op_end:
                if(blockStack){
                    BlockStack* node = blockStack;
                    blockStack = blockStack->next;
                    node->ptr->endOffset = data - begin - node->offset;
                    free_func(node);
                }
                data += sizeof(InstrInst);
                break;
            case Op_unreachable:
            case Op_nop:
            case Op_return:
                data += sizeof(InstrInst);
                break;
            case Op_loop:{
                BlockInstrInst* blockInstr = (BlockInstrInst*)data;
                blockInstr->blocktype = instr->imm.values.value.type;
                blockInstr->index = instr->imm.values.index;
                blockInstr->brOffset = data - begin;
                BlockStack* node = (BlockStack*)malloc_func(sizeof(BlockStack));
                node->next = blockStack;
                node->ptr = blockInstr;
                node->offset = data - begin;
                blockStack = node;
                data += sizeof(BlockInstrInst);
                }break;
            case Op_block:
            case Op_if:{
                BlockInstrInst* blockInstr = (BlockInstrInst*)data;
                blockInstr->blocktype = instr->imm.values.value.type;
                blockInstr->index = instr->imm.values.index;
                blockInstr->brOffset = 0;
                BlockStack* node = (BlockStack*)malloc_func(sizeof(BlockStack));
                node->next = blockStack;
                node->ptr = blockInstr;
                node->offset = data - begin;
                blockStack = node;
                data += sizeof(BlockInstrInst);
                }break;
            case Op_br:
            case Op_br_if:
            case Op_call:
                ((UnaryInstrInst*)data)->index = instr->imm.values.index;
                data += sizeof(UnaryInstrInst);
                break;
            case Op_call_indirect:{
                BinaryInstrInst* binaryInstr = (BinaryInstrInst*)data;
                binaryInstr->index1 = instr->imm.values.index;
                binaryInstr->index2 = instr->imm.values.value.value.u32;
                data += sizeof(BinaryInstrInst);
                }break;
            case Op_br_table:{
                BrTableInstrInst* brTableInstr = (BrTableInstrInst*)data;
                brTableInstr->size = instr->imm.vec.size;
                memcpy_func((u32_t*)brTableInstr->params, instr->imm.vec.data, sizeof(u32_t) * brTableInstr->size);
                data += sizeof(BrTableInstrInst) + (instr->imm.vec.size * sizeof(u32_t));
                }break;
            default:
                break;
        }
    }
}