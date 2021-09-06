#include "parseFuncBody.h"

#include <defines.h>
#include <Opcodes.h>
#include <structures/WasmInstr.h>
#include <error.h>
#include "utils.h"

typedef struct _node {
    WasmInstr data;
    struct _node *next;
} instr_node_t;

typedef struct {
    unsigned int size;
    instr_node_t *head, *end;
} instr_list_t;

int parseControlInstr(u8_t opcode, instr_list_t* const instrs, const byte_t **read_p, const byte_t *end_p)
{
    // Allocate instruction
    instrs->end->next = (instr_node_t*) malloc_func(sizeof(instr_node_t));
    instrs->end = instrs->end->next;
    instrs->end->next = NULL;
    // Fill instr
    WasmInstr* instr = &instrs->end->data;
    instr->opcode = opcode;
    switch (opcode) {
        case Op_block:
        case Op_loop:
        case Op_if:
            switch (*((*read_p)++)) {
                case 0x40:
                    break;
                case TYPE_i32:
                    instr->imm.values.value.type = Value_i32;
                    break;
                case TYPE_i64:
                    instr->imm.values.value.type = Value_i64;
                    break;
                case TYPE_f32:
                    instr->imm.values.value.type = Value_f32;
                    break;
                case TYPE_f64:
                    instr->imm.values.value.type = Value_f64;
                    break;
                case REF_funcref:
                    instr->imm.values.value.type = Value_funcref;
                    break;
                case REF_externref:
                    instr->imm.values.value.type = Value_externref;
                    break;
                default:
                    instr->imm.values.index = getLeb128_u32(read_p, end_p);
                    if(wasmvm_errno) {
                        return -1;
                    }
            }
            break;
        case Op_br:
        case Op_br_if:
        case Op_call:
            instr->imm.values.index = getLeb128_u32(read_p, end_p);
            if(wasmvm_errno) {
                return -1;
            }
            break;
        case Op_br_table:
            instr->imm.vec.size = getLeb128_u32(read_p, end_p);
            if(wasmvm_errno) {
                return -1;
            }
            instr->imm.vec.data = (u32_t*)malloc_func(instr->imm.vec.size * sizeof(u32_t));
            for(u32_t count = 0; count < (instr->imm.vec.size + 1); ++count) {
                ((u32_t*)instr->imm.vec.data)[count] = getLeb128_u32(read_p, end_p);
                if(wasmvm_errno) {
                    return -1;
                }
            }
            break;
        case Op_call_indirect:
            // typeidx
            instr->imm.values.index = getLeb128_u32(read_p, end_p);
            if(wasmvm_errno) {
                return -1;
            }
            // tableidx
            instr->imm.values.value.type = Value_i32;
            instr->imm.values.index = getLeb128_u32(read_p, end_p);
            if(wasmvm_errno) {
                return -1;
            }
            break;
        default:
            break;
    }
    return 0;
}

int parseReferenceInstr(u8_t opcode, instr_list_t* const instrs, const byte_t **read_p, const byte_t *end_p)
{
    // Allocate instruction
    instrs->end->next = (instr_node_t*) malloc_func(sizeof(instr_node_t));
    instrs->end = instrs->end->next;
    instrs->end->next = NULL;
    // Fill instr
    WasmInstr* instr = &instrs->end->data;
    instr->opcode = opcode;
    switch (opcode) {
        case Op_ref_null:
            switch (*((*read_p)++)) {
                case REF_funcref:
                    instr->imm.values.value.type = Value_funcref;
                    break;
                case REF_externref:
                    instr->imm.values.value.type = Value_externref;
                    break;
                default:
                    wasmvm_errno = ERROR_unexpected_token;
                    return -1;
            }
            break;
        case Op_ref_func:
            instr->imm.values.index = getLeb128_u32(read_p, end_p);
            if(wasmvm_errno) {
                return -1;
            }
            break;
        default:
            break;
    }
    return 0;
}

int parseParametricInstr(u8_t opcode, instr_list_t* const instrs, const byte_t **read_p, const byte_t *end_p)
{
    // Allocate instruction
    instrs->end->next = (instr_node_t*) malloc_func(sizeof(instr_node_t));
    instrs->end = instrs->end->next;
    instrs->end->next = NULL;
    // Fill instr
    WasmInstr* instr = &instrs->end->data;
    instr->opcode = opcode;
    if (opcode == Op_select_t) {
        instr->imm.vec.size = getLeb128_u32(read_p, end_p);
        if(wasmvm_errno) {
            return -1;
        }
        for(u32_t index = 0; index < instr->imm.vec.size; ++index) {
            switch (*((*read_p)++)) {
                case TYPE_i32:
                    instr->imm.values.value.type = Value_i32;
                    break;
                case TYPE_i64:
                    instr->imm.values.value.type = Value_i64;
                    break;
                case TYPE_f32:
                    instr->imm.values.value.type = Value_f32;
                    break;
                case TYPE_f64:
                    instr->imm.values.value.type = Value_f64;
                    break;
                case REF_funcref:
                    instr->imm.values.value.type = Value_funcref;
                    break;
                case REF_externref:
                    instr->imm.values.value.type = Value_externref;
                    break;
                default:
                    wasmvm_errno = ERROR_unexpected_token;
                    return -1;
            }
        }
    }
    return 0;
}

int parseVariableInstr(u8_t opcode, instr_list_t* const instrs, const byte_t **read_p, const byte_t *end_p)
{
    // Allocate instruction
    instrs->end->next = (instr_node_t*) malloc_func(sizeof(instr_node_t));
    instrs->end = instrs->end->next;
    instrs->end->next = NULL;
    // Fill instr
    WasmInstr* instr = &instrs->end->data;
    instr->opcode = opcode;
    switch (opcode) {
        case Op_ref_null:
            switch (*((*read_p)++)) {
                case REF_funcref:
                    instr->imm.values.value.type = Value_funcref;
                    break;
                case REF_externref:
                    instr->imm.values.value.type = Value_externref;
                    break;
                default:
                    wasmvm_errno = ERROR_unexpected_token;
                    return -1;
            }
            break;
        case Op_ref_func:
            instr->imm.values.index = getLeb128_u32(read_p, end_p);
            if(wasmvm_errno) {
                return -1;
            }
            break;
        default:
            break;
    }
    return 0;
}

int parseTableInstr(u8_t opcode, instr_list_t* const instrs, const byte_t **read_p, const byte_t *end_p)
{
    // Allocate instruction
    instrs->end->next = (instr_node_t*) malloc_func(sizeof(instr_node_t));
    instrs->end = instrs->end->next;
    instrs->end->next = NULL;
    // Fill instr
    WasmInstr* instr = &instrs->end->data;
    instr->opcode = opcode;
    instr->imm.values.index = getLeb128_u32(read_p, end_p);
    if(wasmvm_errno) {
        return -1;
    }
    if((opcode == Op_table_init) || (opcode == Op_table_copy)) {
        instr->imm.values.value.type = Value_i32;
        instr->imm.values.value.value.u32 = getLeb128_u32(read_p, end_p);
        if(wasmvm_errno) {
            return -1;
        }
    }
    return 0;
}

int parseFuncBody(WasmFunc* const func, const byte_t **read_p, const byte_t *end_p)
{
    // Initialize instruction list
    instr_list_t instrs;
    instrs.size = 0;
    instrs.head = NULL;
    instrs.end = NULL;
    // Parse instructions
    while(*read_p != end_p) {
        u16_t opcode = *((*read_p)++);
        if(opcode == 0xFC) {
            opcode = (opcode << 8) | *((*read_p)++);
        }
        switch (opcode) {
            case Op_nop:
            case Op_unreachable:
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
                if(parseControlInstr(opcode, &instrs, read_p, end_p)) {
                    return -1;
                }
                break;
            case Op_ref_null:
            case Op_ref_is_null:
            case Op_ref_func:
                if(parseReferenceInstr(opcode, &instrs, read_p, end_p)) {
                    return -1;
                }
                break;
            case Op_drop:
            case Op_select:
            case Op_select_t:
                if(parseParametricInstr(opcode, &instrs, read_p, end_p)) {
                    return -1;
                }
                break;
            case Op_local_get:
            case Op_local_set:
            case Op_local_tee:
            case Op_global_get:
            case Op_global_set:
                if(parseVariableInstr(opcode, &instrs, read_p, end_p)) {
                    return -1;
                }
                break;
            case Op_table_set:
            case Op_table_get:
            case Op_table_init:
            case Op_elem_drop:
            case Op_table_copy:
            case Op_table_grow:
            case Op_table_size:
            case Op_table_fill:
                if(parseTableInstr(opcode, &instrs, read_p, end_p)) {
                    return -1;
                }
                break;
            //     case Op_i32_load:
            //     case Op_i64_load:
            //     case Op_f32_load:
            //     case Op_f64_load:
            //     case Op_i32_load8_s:
            //     case Op_i32_load8_u:
            //     case Op_i32_load16_s:
            //     case Op_i32_load16_u:
            //     case Op_i64_load8_s:
            //     case Op_i64_load8_u:
            //     case Op_i64_load16_s:
            //     case Op_i64_load16_u:
            //     case Op_i64_load32_s:
            //     case Op_i64_load32_u:
            //     case Op_i32_store:
            //     case Op_i64_store:
            //     case Op_f32_store:
            //     case Op_f64_store:
            //     case Op_i32_store8:
            //     case Op_i32_store16:
            //     case Op_i64_store8:
            //     case Op_i64_store16:
            //     case Op_i64_store32:
            //     case Op_memory_size:
            //     case Op_memory_grow:
            //         instr = (WasmInstr*)parseMemoryInstr(opcode, read_p, end_p);
            //         break;
            //     case Op_i32_const:
            //     case Op_i64_const:
            //     case Op_f32_const:
            //     case Op_f64_const:
            //     case Op_i32_eqz:
            //     case Op_i32_eq:
            //     case Op_i32_ne:
            //     case Op_i32_lt_s:
            //     case Op_i32_lt_u:
            //     case Op_i32_gt_s:
            //     case Op_i32_gt_u:
            //     case Op_i32_le_s:
            //     case Op_i32_le_u:
            //     case Op_i32_ge_s:
            //     case Op_i32_ge_u:
            //     case Op_i64_eqz:
            //     case Op_i64_eq:
            //     case Op_i64_ne:
            //     case Op_i64_lt_s:
            //     case Op_i64_lt_u:
            //     case Op_i64_gt_s:
            //     case Op_i64_gt_u:
            //     case Op_i64_le_s:
            //     case Op_i64_le_u:
            //     case Op_i64_ge_s:
            //     case Op_i64_ge_u:
            //     case Op_f32_eq:
            //     case Op_f32_ne:
            //     case Op_f32_lt:
            //     case Op_f32_gt:
            //     case Op_f32_le:
            //     case Op_f32_ge:
            //     case Op_f64_eq:
            //     case Op_f64_ne:
            //     case Op_f64_lt:
            //     case Op_f64_gt:
            //     case Op_f64_le:
            //     case Op_f64_ge:
            //     case Op_i32_clz:
            //     case Op_i32_ctz:
            //     case Op_i32_popcnt:
            //     case Op_i32_add:
            //     case Op_i32_sub:
            //     case Op_i32_mul:
            //     case Op_i32_div_s:
            //     case Op_i32_div_u:
            //     case Op_i32_rem_s:
            //     case Op_i32_rem_u:
            //     case Op_i32_and:
            //     case Op_i32_or:
            //     case Op_i32_xor:
            //     case Op_i32_shl:
            //     case Op_i32_shr_s:
            //     case Op_i32_shr_u:
            //     case Op_i32_rotl:
            //     case Op_i32_rotr:
            //     case Op_i64_clz:
            //     case Op_i64_ctz:
            //     case Op_i64_popcnt:
            //     case Op_i64_add:
            //     case Op_i64_sub:
            //     case Op_i64_mul:
            //     case Op_i64_div_s:
            //     case Op_i64_div_u:
            //     case Op_i64_rem_s:
            //     case Op_i64_rem_u:
            //     case Op_i64_and:
            //     case Op_i64_or:
            //     case Op_i64_xor:
            //     case Op_i64_shl:
            //     case Op_i64_shr_s:
            //     case Op_i64_shr_u:
            //     case Op_i64_rotl:
            //     case Op_i64_rotr:
            //     case Op_f32_abs:
            //     case Op_f32_neg:
            //     case Op_f32_ceil:
            //     case Op_f32_floor:
            //     case Op_f32_trunc:
            //     case Op_f32_nearest:
            //     case Op_f32_sqrt:
            //     case Op_f32_add:
            //     case Op_f32_sub:
            //     case Op_f32_mul:
            //     case Op_f32_div:
            //     case Op_f32_min:
            //     case Op_f32_max:
            //     case Op_f32_copysign:
            //     case Op_f64_abs:
            //     case Op_f64_neg:
            //     case Op_f64_ceil:
            //     case Op_f64_floor:
            //     case Op_f64_trunc:
            //     case Op_f64_nearest:
            //     case Op_f64_sqrt:
            //     case Op_f64_add:
            //     case Op_f64_sub:
            //     case Op_f64_mul:
            //     case Op_f64_div:
            //     case Op_f64_min:
            //     case Op_f64_max:
            //     case Op_f64_copysign:
            //     case Op_i32_wrap_i64:
            //     case Op_i32_trunc_s_f32:
            //     case Op_i32_trunc_u_f32:
            //     case Op_i32_trunc_s_f64:
            //     case Op_i32_trunc_u_f64:
            //     case Op_i64_extend_s_i32:
            //     case Op_i64_extend_u_i32:
            //     case Op_i64_trunc_s_f32:
            //     case Op_i64_trunc_u_f32:
            //     case Op_i64_trunc_s_f64:
            //     case Op_i64_trunc_u_f64:
            //     case Op_f32_convert_s_i32:
            //     case Op_f32_convert_u_i32:
            //     case Op_f32_convert_s_i64:
            //     case Op_f32_convert_u_i64:
            //     case Op_f32_demote_f64:
            //     case Op_f64_convert_s_i32:
            //     case Op_f64_convert_u_i32:
            //     case Op_f64_convert_s_i64:
            //     case Op_f64_convert_u_i64:
            //     case Op_f64_promote_f32:
            //     case Op_i32_reinterpret_f32:
            //     case Op_i64_reinterpret_f64:
            //     case Op_f32_reinterpret_i32:
            //     case Op_f64_reinterpret_i64:
            //         instr = (WasmInstr*)parseNumericInstr(opcode, read_p, end_p);
            //         break;
            default:
                return -1;
        }
    }
    return 0;
}

// WasmNumericInstr* parseNumericInstr(uint8_t opcode, uint8_t **read_p, const uint8_t *end_p)
// {
//     WasmNumericInstr* instr = new_WasmNumericInstr();
//     instr->parent.opcode = opcode;
//     switch (opcode) {
//         case Op_i32_const:
//             instr->constant.type = Value_i32;
//             instr->constant.value.i32 = getLeb128_i32(read_p, end_p);
//             break;
//         case Op_i64_const:
//             instr->constant.type = Value_i64;
//             instr->constant.value.i64 = getLeb128_i64(read_p, end_p);
//             break;
//         case Op_f32_const:
//             instr->constant.type = Value_f32;
//             instr->constant.value.i32 = toLittle32(*((uint32_t*)*read_p), 0);
//             *read_p += 4;
//             break;
//         case Op_f64_const:
//             instr->constant.type = Value_f64;
//             instr->constant.value.i64 = toLittle64(*((uint32_t*)*read_p), 0);
//             *read_p += 8;
//             break;
//         default:
//             break;
//     }
//     return instr;
// }
// WasmParametricInstr* parseParametricInstr(uint8_t opcode, uint8_t **read_p, const uint8_t *end_p)
// {
//     WasmParametricInstr* instr = new_WasmParametricInstr();
//     instr->parent.opcode = opcode;
//     return instr;
// }
// WasmVariableInstr* parseVariableInstr(uint8_t opcode, uint8_t **read_p, const uint8_t *end_p)
// {
//     WasmVariableInstr* instr = new_WasmVariableInstr(getLeb128_u32(read_p, end_p));
//     instr->parent.opcode = opcode;
//     return instr;
// }
// WasmMemoryInstr* parseMemoryInstr(uint8_t opcode, uint8_t **read_p, const uint8_t *end_p)
// {
//     WasmMemoryInstr* instr = NULL;
//     switch (opcode) {
//         case Op_memory_size:
//         case Op_memory_grow:
//             if(*((*read_p)++) != 0x00) {
//                 return NULL;
//             }
//             instr = new_WasmMemoryInstr(0, 0);
//             break;
//         default: {
//             uint32_t align = getLeb128_u32(read_p, end_p);
//             uint32_t offset = getLeb128_u32(read_p, end_p);
//             instr = new_WasmMemoryInstr(align, offset);
//         }
//         break;

//     }
//     instr->parent.opcode = opcode;
//     return instr;
// }
