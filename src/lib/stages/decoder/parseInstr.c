#include "parseInstr.h"

#include <Opcodes.h>
#include "sections.h"
#include "Utils.h"

int parseInstr(WasmFunc *func, uint8_t **read_p, const uint8_t *end_p)
{
    uint8_t opcode = *((*read_p)++);
    WasmInstr* instr = NULL;
    switch (opcode) {
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
            instr = (WasmInstr*)parseControlInstr(opcode, read_p, end_p);
            break;
        case Op_drop:
        case Op_select:
            instr = (WasmInstr*)parseParametricInstr(opcode, read_p, end_p);
            break;
        case Op_get_local:
        case Op_set_local:
        case Op_tee_local:
        case Op_get_global:
        case Op_set_global:
            instr = (WasmInstr*)parseVariableInstr(opcode, read_p, end_p);
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
            instr = (WasmInstr*)parseMemoryInstr(opcode, read_p, end_p);
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
            instr = (WasmInstr*)parseNumericInstr(opcode, read_p, end_p);
            break;
        default:
            return -1;
    }
    if(instr) {
        list_push_back(func->body, instr);
        return 0;
    }
    return -2;
}
WasmNumericInstr* parseNumericInstr(uint8_t opcode, uint8_t **read_p, const uint8_t *end_p)
{
    WasmNumericInstr* instr = new_WasmNumericInstr();
    instr->parent.opcode = opcode;
    switch (opcode) {
        case Op_i32_const:
            instr->constant.type = Value_i32;
            instr->constant.value.i32 = getLeb128_i32(read_p, end_p);
            break;
        case Op_i64_const:
            instr->constant.type = Value_i64;
            instr->constant.value.i64 = getLeb128_i64(read_p, end_p);
            break;
        case Op_f32_const:
            instr->constant.type = Value_f32;
            instr->constant.value.i32 = toLittle32(*((uint32_t*)*read_p), 0);
            *read_p += 4;
            break;
        case Op_f64_const:
            instr->constant.type = Value_f64;
            instr->constant.value.i64 = toLittle64(*((uint32_t*)*read_p), 0);
            *read_p += 8;
            break;
        default:
            break;
    }
    return instr;
}
WasmParametricInstr* parseParametricInstr(uint8_t opcode, uint8_t **read_p, const uint8_t *end_p)
{
    WasmParametricInstr* instr = new_WasmParametricInstr();
    instr->parent.opcode = opcode;
    return instr;
}
WasmVariableInstr* parseVariableInstr(uint8_t opcode, uint8_t **read_p, const uint8_t *end_p)
{
    WasmVariableInstr* instr = new_WasmVariableInstr(getLeb128_u32(read_p, end_p));
    instr->parent.opcode = opcode;
    return instr;
}
WasmMemoryInstr* parseMemoryInstr(uint8_t opcode, uint8_t **read_p, const uint8_t *end_p)
{
    WasmMemoryInstr* instr = NULL;
    switch (opcode) {
        case Op_memory_size:
        case Op_memory_grow:
            if(*((*read_p)++) != 0x00) {
                return NULL;
            }
            instr = new_WasmMemoryInstr(0, 0);
            break;
        default: {
            uint32_t align = getLeb128_u32(read_p, end_p);
            uint32_t offset = getLeb128_u32(read_p, end_p);
            instr = new_WasmMemoryInstr(align, offset);
        }
        break;

    }
    instr->parent.opcode = opcode;
    return instr;
}
WasmControlInstr* parseControlInstr(uint8_t opcode, uint8_t **read_p, const uint8_t *end_p)
{
    WasmControlInstr* instr = new_WasmControlInstr();
    instr->parent.opcode = opcode;
    switch (opcode) {
        case Op_block:
        case Op_loop:
        case Op_if: {
            ValueType resultType;
            switch (*((*read_p)++)) {
                case 0x40:
                    break;
                case TYPE_i32:
                    resultType = Value_i32;
                    vector_push_back(instr->resultTypes, &resultType);
                    break;
                case TYPE_i64:
                    resultType = Value_i64;
                    vector_push_back(instr->resultTypes, &resultType);
                    break;
                case TYPE_f32:
                    resultType = Value_f32;
                    vector_push_back(instr->resultTypes, &resultType);
                    break;
                case TYPE_f64:
                    resultType = Value_f64;
                    vector_push_back(instr->resultTypes, &resultType);
                    break;
                default:
                    free_WasmControlInstr(instr);
                    return NULL;
            }
        }
        break;
        case Op_br:
        case Op_br_if:
        case Op_call: {
            uint32_t index = getLeb128_u32(read_p, end_p);
            vector_push_back(instr->indices, &index);
        }
        break;
        case Op_br_table: {
            for(uint32_t targetNum = getLeb128_u32(read_p, end_p); targetNum > 0; --targetNum) {
                uint32_t index = getLeb128_u32(read_p, end_p);
                vector_push_back(instr->indices, &index);
            }
            uint32_t defaultIndex = getLeb128_u32(read_p, end_p);
            vector_push_back(instr->indices, &defaultIndex);
        }
        break;
        case Op_call_indirect: {
            uint32_t index = getLeb128_u32(read_p, end_p);
            vector_push_back(instr->indices, &index);
            if(*((*read_p)++) != 0x00) {
                free_WasmControlInstr(instr);
                return NULL;
            }
        }
        break;
        default:
            break;
    }
    return instr;
}
