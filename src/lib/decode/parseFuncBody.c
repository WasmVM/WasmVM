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

// Allocate instruction
#define alloc_instr() \
    if(instrs->head == NULL){ \
        instrs->head = (instr_node_t*) malloc_func(sizeof(instr_node_t)); \
        instrs->end = instrs->head; \
    }else{ \
        instrs->end->next = (instr_node_t*) malloc_func(sizeof(instr_node_t)); \
        instrs->end = instrs->end->next; \
    } \
    instrs->end->next = NULL;

int parseControlInstr(u16_t opcode, instr_list_t* const instrs, const byte_t **read_p, const byte_t *end_p)
{
    alloc_instr()
    // Fill instr
    WasmInstr* instr = &(instrs->end->data);
    instr->opcode = opcode;
    switch (opcode) {
        case Op_block:
        case Op_loop:
        case Op_if:
            switch (**read_p) {
                case 0x40:
                    (*read_p)++;
                    break;
                case TYPE_i32:
                    instr->imm.values.value.type = Value_i32;
                    (*read_p)++;
                    break;
                case TYPE_i64:
                    instr->imm.values.value.type = Value_i64;
                    (*read_p)++;
                    break;
                case TYPE_f32:
                    instr->imm.values.value.type = Value_f32;
                    (*read_p)++;
                    break;
                case TYPE_f64:
                    instr->imm.values.value.type = Value_f64;
                    (*read_p)++;
                    break;
                case REF_funcref:
                    instr->imm.values.value.type = Value_funcref;
                    (*read_p)++;
                    break;
                case REF_externref:
                    instr->imm.values.value.type = Value_externref;
                    (*read_p)++;
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
            instr->imm.vec.size = getLeb128_u32(read_p, end_p) + 1;
            if(wasmvm_errno) {
                return -1;
            }
            instr->imm.vec.data = (u32_t*)malloc_func(instr->imm.vec.size * sizeof(u32_t));
            for(u32_t count = 0; count < instr->imm.vec.size; ++count) {
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
            if(*((*read_p)++) != 0x00) {
                // typeidx should be zero now
                wasmvm_errno = ERROR_zero_expected;
                return -1;
            }
            instr->imm.values.value.type = Value_i32;
            instr->imm.values.value.value.u32 = 0;
            break;
        default:
            break;
    }
    return 0;
}

int parseReferenceInstr(u16_t opcode, instr_list_t* const instrs, const byte_t **read_p, const byte_t *end_p)
{
    alloc_instr()
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

int parseParametricInstr(u16_t opcode, instr_list_t* const instrs, const byte_t **read_p, const byte_t *end_p)
{
    alloc_instr()
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

int parseVariableInstr(u16_t opcode, instr_list_t* const instrs, const byte_t **read_p, const byte_t *end_p)
{
    alloc_instr()
    // Fill instr
    WasmInstr* instr = &instrs->end->data;
    instr->opcode = opcode;
    instr->imm.values.index = getLeb128_u32(read_p, end_p);
    if(wasmvm_errno) {
        return -1;
    }
    return 0;
}

int parseTableInstr(u16_t opcode, instr_list_t* const instrs, const byte_t **read_p, const byte_t *end_p)
{
    alloc_instr()
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

int parseMemoryInstr(u16_t opcode, instr_list_t* const instrs, const byte_t **read_p, const byte_t *end_p)
{
    alloc_instr()
    // Fill instr
    WasmInstr* instr = &instrs->end->data;
    instr->opcode = opcode;
    switch (opcode) {
        case Op_memory_init:
            instr->imm.values.index = getLeb128_u32(read_p, end_p);
            if(wasmvm_errno) {
                return -1;
            }
            if(*((*read_p)++) != 0x00) {
                wasmvm_errno = ERROR_unexpected_token;
                return -1;
            }
            break;
        case Op_data_drop:
            instr->imm.values.index = getLeb128_u32(read_p, end_p);
            if(wasmvm_errno) {
                return -1;
            }
            break;
        case Op_memory_copy:
            if(*((*read_p)++) != 0x00) {
                wasmvm_errno = ERROR_unexpected_token;
                return -1;
            }
        case Op_memory_size:
        case Op_memory_grow:
        case Op_memory_fill:
            if(*((*read_p)++) != 0x00) {
                wasmvm_errno = ERROR_zero_expected;
                return -1;
            }
            break;
        default:
            instr->imm.memarg.align = getLeb128_u32(read_p, end_p);
            if(wasmvm_errno) {
                return -1;
            }
            instr->imm.memarg.offset = getLeb128_u32(read_p, end_p);
            if(wasmvm_errno) {
                return -1;
            }
            break;
    }
    return 0;
}

int parseNumericInstr(u16_t opcode, instr_list_t* const instrs, const byte_t **read_p, const byte_t *end_p)
{
    alloc_instr()
    // Fill instr
    WasmInstr* instr = &instrs->end->data;
    instr->opcode = opcode;
    switch (opcode) {
        case Op_i32_const:
            instr->imm.values.value.type = Value_i32;
            instr->imm.values.value.value.i32 = getLeb128_i32(read_p, end_p);
            break;
        case Op_i64_const:
            instr->imm.values.value.type = Value_i64;
            instr->imm.values.value.value.i64 = getLeb128_i64(read_p, end_p);
            break;
        case Op_f32_const:
            instr->imm.values.value.type = Value_f32;
            instr->imm.values.value.value.u32 = toLittle32(*((u32_t*)*read_p), 0);
            *read_p += 4;
            break;
        case Op_f64_const:
            instr->imm.values.value.type = Value_f64;
            instr->imm.values.value.value.u64 = toLittle64(*((u64_t*)*read_p), 0);
            *read_p += 8;
            break;
        default:
            break;
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
            u32_t extOp = getLeb128_u32(read_p, end_p);
            if(wasmvm_errno) {
                return -1;
            }
            opcode = (opcode << 8) | ((u16_t)extOp);
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
                if(parseMemoryInstr(opcode, &instrs, read_p, end_p)) {
                    return -1;
                }
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
                if(parseNumericInstr(opcode, &instrs, read_p, end_p)) {
                    return -1;
                }
                break;
            default:
                wasmvm_errno = ERROR_unexpected_token;
                return -1;
        }
    }
    // Allocate memory
    if(instrs.size > 0) {
        func->body.data = (WasmInstr*)malloc_func(sizeof(WasmInstr) * instrs.size);
    }
    func->body.size = instrs.size;
    // Copy instrs
    for (unsigned int i = 0; i < instrs.size; ++i) {
        instr_node_t* cur = instrs.head;
        instrs.head = instrs.head->next;
        func->body.data[i] = cur->data;
        free_func(cur);
    }

    return 0;
}
