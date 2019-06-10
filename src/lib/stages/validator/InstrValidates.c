#include "Validates.h"

#include <dataTypes/stack_p.h>
#include <Opcodes.h>
#include <stdlib.h>

#include <stdio.h>

static int pop_opd(stack_p opds, stack_p ctrls, ValueType** operand)
{
    ctrl_frame* frame = stack_top(ctrl_frame*, ctrls);
    if(stack_size(opds) == frame->height) {
        if(frame->unreachable) {
            *operand = (ValueType*) malloc(sizeof(ValueType));
            **operand = Value_Unspecified;
            return 0;
        } else {
            return -1;
        }
    }
    *operand = stack_pop(ValueType*, opds);
    return 0;
}

static int pop_opd_expect(stack_p opds, stack_p ctrls, ValueType** operand, ValueType expect)
{
    ValueType* actual = NULL;
    if(pop_opd(opds, ctrls, &actual)) {
        return -1;
    }
    if(*actual == Value_Unspecified) {
        *actual = expect;
    } else {
        if(expect != Value_Unspecified && *actual != expect) {
            free(actual);
            return -2;
        }
    }
    *operand = actual;
    return 0;
}

static ctrl_frame* ctrl_at(stack_p ctrls, size_t index)
{
    if(stack_size(ctrls) <= index) {
        return NULL;
    }
    stack_iterator cursor = stack_head(ctrls);
    for(size_t i = 0; i < index; ++i) {
        cursor = stack_iterator_next(cursor);
    }
    return stack_iterator_get(ctrl_frame*, cursor);
}

int validate_Instr_const(WasmNumericInstr* instr, Context* context, stack_p opds, stack_p ctrls)
{
    ValueType* type = (ValueType*) malloc(sizeof(ValueType));
    *type = instr->constant.type;
    stack_push(opds, type);
    return 0;
}
int validate_Instr_unop(WasmNumericInstr* instr, Context* context, stack_p opds, stack_p ctrls)
{
    ValueType expect;
    switch(instr->parent.opcode) {
        case Op_i32_clz:
        case Op_i32_ctz:
        case Op_i32_popcnt:
            expect = Value_i32;
            break;
        case Op_i64_clz:
        case Op_i64_ctz:
        case Op_i64_popcnt:
            expect = Value_i64;
            break;
        case Op_f32_abs:
        case Op_f32_neg:
        case Op_f32_ceil:
        case Op_f32_floor:
        case Op_f32_trunc:
        case Op_f32_nearest:
        case Op_f32_sqrt:
            expect = Value_f32;
            break;
        case Op_f64_abs:
        case Op_f64_neg:
        case Op_f64_ceil:
        case Op_f64_floor:
        case Op_f64_trunc:
        case Op_f64_nearest:
        case Op_f64_sqrt:
            expect = Value_f64;
            break;
        default:
            return -1;
    }
    ValueType* operand = NULL;
    if(pop_opd_expect(opds, ctrls, &operand, expect)) {
        return -2;
    }
    stack_push(opds, operand);
    return 0;
}
int validate_Instr_binop(WasmNumericInstr* instr, Context* context, stack_p opds, stack_p ctrls)
{
    ValueType expect;
    switch(instr->parent.opcode) {
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
            expect = Value_i32;
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
            expect = Value_i64;
            break;
        case Op_f32_add:
        case Op_f32_sub:
        case Op_f32_mul:
        case Op_f32_div:
        case Op_f32_min:
        case Op_f32_max:
        case Op_f32_copysign:
            expect = Value_f32;
            break;
        case Op_f64_add:
        case Op_f64_sub:
        case Op_f64_mul:
        case Op_f64_div:
        case Op_f64_min:
        case Op_f64_max:
        case Op_f64_copysign:
            expect = Value_f64;
            break;
        default:
            return -1;
    }
    ValueType* operand1 = NULL;
    if(pop_opd_expect(opds, ctrls, &operand1, expect)) {
        return -2;
    }
    ValueType* operand2 = NULL;
    if(pop_opd_expect(opds, ctrls, &operand2, expect)) {
        return -3;
    }
    stack_push(opds, operand1);
    free(operand2);
    return 0;
}
int validate_Instr_testop(WasmNumericInstr* instr, Context* context, stack_p opds, stack_p ctrls)
{
    ValueType expect;
    switch(instr->parent.opcode) {
        case Op_i32_eqz:
            expect = Value_i32;
            break;
        case Op_i64_eqz:
            expect = Value_i64;
            break;
        default:
            return -1;
    }
    ValueType* operand = NULL;
    if(pop_opd_expect(opds, ctrls, &operand, expect)) {
        return -2;
    }
    *operand = Value_i32;
    stack_push(opds, operand);
    return 0;
}
int validate_Instr_relop(WasmNumericInstr* instr, Context* context, stack_p opds, stack_p ctrls)
{
    ValueType expect;
    switch(instr->parent.opcode) {
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
            expect = Value_i32;
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
            expect = Value_i64;
            break;
        case Op_f32_eq:
        case Op_f32_ne:
        case Op_f32_lt:
        case Op_f32_gt:
        case Op_f32_le:
        case Op_f32_ge:
            expect = Value_f32;
            break;
        case Op_f64_eq:
        case Op_f64_ne:
        case Op_f64_lt:
        case Op_f64_gt:
        case Op_f64_le:
        case Op_f64_ge:
            expect = Value_f64;
            break;
        default:
            return -1;
    }
    ValueType* operand1 = NULL;
    if(pop_opd_expect(opds, ctrls, &operand1, expect)) {
        return -2;
    }
    ValueType* operand2 = NULL;
    if(pop_opd_expect(opds, ctrls, &operand2, expect)) {
        return -3;
    }
    *operand1 = Value_i32;
    stack_push(opds, operand1);
    free(operand2);
    return 0;
}
int validate_Instr_cvtop(WasmNumericInstr* instr, Context* context, stack_p opds, stack_p ctrls)
{
    ValueType expect;
    ValueType result;
    switch(instr->parent.opcode) {
        case Op_i32_wrap_i64:
            expect = Value_i64;
            result = Value_i32;
            break;
        case Op_i32_trunc_s_f32:
        case Op_i32_trunc_u_f32:
            expect = Value_f32;
            result = Value_i32;
            break;
        case Op_i32_trunc_s_f64:
        case Op_i32_trunc_u_f64:
            expect = Value_f64;
            result = Value_i32;
            break;
        case Op_i64_extend_s_i32:
        case Op_i64_extend_u_i32:
            expect = Value_i32;
            result = Value_i64;
            break;
        case Op_i64_trunc_s_f32:
        case Op_i64_trunc_u_f32:
            expect = Value_f32;
            result = Value_i64;
            break;
        case Op_i64_trunc_s_f64:
        case Op_i64_trunc_u_f64:
            expect = Value_f64;
            result = Value_i64;
            break;
        case Op_f32_convert_s_i32:
        case Op_f32_convert_u_i32:
            expect = Value_i32;
            result = Value_f32;
            break;
        case Op_f32_convert_s_i64:
        case Op_f32_convert_u_i64:
            expect = Value_i64;
            result = Value_f32;
            break;
        case Op_f32_demote_f64:
            expect = Value_f64;
            result = Value_f32;
            break;
        case Op_f64_convert_s_i32:
        case Op_f64_convert_u_i32:
            expect = Value_i32;
            result = Value_f64;
            break;
        case Op_f64_convert_s_i64:
        case Op_f64_convert_u_i64:
            expect = Value_i64;
            result = Value_f64;
            break;
        case Op_f64_promote_f32:
            expect = Value_f32;
            result = Value_f64;
            break;
        case Op_i32_reinterpret_f32:
            expect = Value_f32;
            result = Value_i32;
            break;
        case Op_i64_reinterpret_f64:
            expect = Value_f64;
            result = Value_i64;
            break;
        case Op_f32_reinterpret_i32:
            expect = Value_i32;
            result = Value_f32;
            break;
        case Op_f64_reinterpret_i64:
            expect = Value_i64;
            result = Value_f64;
            break;
        default:
            return -1;
    }
    ValueType* operand = NULL;
    if(pop_opd_expect(opds, ctrls, &operand, expect)) {
        return -2;
    }
    *operand = result;
    stack_push(opds, operand);
    return 0;
}
int validate_Instr_drop(WasmParametricInstr* instr, Context* context, stack_p opds, stack_p ctrls)
{
    ValueType* operand = NULL;
    if(pop_opd(opds, ctrls, &operand)) {
        return -1;
    }
    free(operand);
    return 0;
}
int validate_Instr_select(WasmParametricInstr* instr, Context* context, stack_p opds, stack_p ctrls)
{
    ValueType* condition = NULL;
    if(pop_opd_expect(opds, ctrls, &condition, Value_i32)) {
        return -1;
    }
    ValueType* operand1 = NULL;
    if(pop_opd(opds, ctrls, &operand1)) {
        free(condition);
        return -2;
    }
    ValueType* operand2 = NULL;
    if(pop_opd_expect(opds, ctrls, &operand2, *operand1)) {
        free(condition);
        free(operand1);
        return -3;
    }
    stack_push(opds, operand1);
    free(operand2);
    free(condition);
    return 0;
}
int validate_Instr_get_local(WasmVariableInstr* instr, Context* context, stack_p opds, stack_p ctrls)
{
    if(instr->index >= vector_size(context->locals)) {
        return -1;
    }
    ValueType* local = vector_at(ValueType*, context->locals, instr->index);
    ValueType* operand = (ValueType*) malloc(sizeof(ValueType));
    *operand = *local;
    stack_push(opds, operand);
    return 0;
}
int validate_Instr_set_local(WasmVariableInstr* instr, Context* context, stack_p opds, stack_p ctrls)
{
    if(instr->index >= vector_size(context->locals)) {
        return -1;
    }
    ValueType* operand = NULL;
    if(pop_opd(opds, ctrls, &operand)) {
        return -2;
    }
    free(operand);
    return 0;
}
int validate_Instr_tee_local(WasmVariableInstr* instr, Context* context, stack_p opds, stack_p ctrls)
{
    if(instr->index >= vector_size(context->locals)) {
        return -1;
    }
    ValueType* operand = NULL;
    if(pop_opd(opds, ctrls, &operand)) {
        return -2;
    }
    stack_push(opds, operand);
    return 0;
}
int validate_Instr_get_global(WasmVariableInstr* instr, Context* context, stack_p opds, stack_p ctrls)
{
    if(instr->index >= vector_size(context->module->globals)) {
        return -1;
    }
    WasmGlobal* global = vector_at(WasmGlobal*, context->module->globals, instr->index);
    ValueType* operand = (ValueType*) malloc(sizeof(ValueType));
    *operand = global->valType;
    stack_push(opds, operand);
    return 0;
}
int validate_Instr_set_global(WasmVariableInstr* instr, Context* context, stack_p opds, stack_p ctrls)
{
    if(instr->index >= vector_size(context->module->globals)) {
        return -1;
    }
    WasmGlobal* global = vector_at(WasmGlobal*, context->module->globals, instr->index);
    if(!global->mut) {
        return -2;
    }
    ValueType* operand = NULL;
    if(pop_opd(opds, ctrls, &operand)) {
        return -3;
    }
    free(operand);
    return 0;
}
int validate_Instr_load(WasmMemoryInstr* instr, Context* context, stack_p opds, stack_p ctrls)
{
    if(vector_size(context->module->mems) < 1) {
        return -1;
    }
    uint32_t maxAlign = 0;
    ValueType resultType;
    switch(instr->parent.opcode) {
        case Op_i32_load:
            maxAlign = 2;
            resultType = Value_i32;
            break;
        case Op_i64_load:
            maxAlign = 3;
            resultType = Value_i64;
            break;
        case Op_f32_load:
            maxAlign = 2;
            resultType = Value_f32;
            break;
        case Op_f64_load:
            maxAlign = 3;
            resultType = Value_f64;
            break;
        default:
            return -2;
    }
    if(instr->align > maxAlign) {
        return -3;
    }
    ValueType* operand = NULL;
    if(pop_opd_expect(opds, ctrls, &operand, Value_i32)) {
        return -4;
    }
    *operand = resultType;
    stack_push(opds, operand);
    return 0;
}
int validate_Instr_loadN(WasmMemoryInstr* instr, Context* context, stack_p opds, stack_p ctrls)
{
    if(vector_size(context->module->mems) < 1) {
        return -1;
    }
    uint32_t maxAlign = 0;
    ValueType resultType;
    switch(instr->parent.opcode) {
        case Op_i32_load8_s:
        case Op_i32_load8_u:
            maxAlign = 0;
            resultType = Value_i32;
            break;
        case Op_i32_load16_s:
        case Op_i32_load16_u:
            maxAlign = 1;
            resultType = Value_i32;
            break;
        case Op_i64_load8_s:
        case Op_i64_load8_u:
            maxAlign = 0;
            resultType = Value_i64;
            break;
        case Op_i64_load16_s:
        case Op_i64_load16_u:
            maxAlign = 1;
            resultType = Value_i64;
            break;
        case Op_i64_load32_s:
        case Op_i64_load32_u:
            maxAlign = 2;
            resultType = Value_i64;
            break;
        default:
            return -2;
    }
    if(instr->align > maxAlign) {
        return -3;
    }
    ValueType* operand = NULL;
    if(pop_opd_expect(opds, ctrls, &operand, Value_i32)) {
        return -4;
    }
    *operand = resultType;
    stack_push(opds, operand);
    return 0;
}
int validate_Instr_store(WasmMemoryInstr* instr, Context* context, stack_p opds, stack_p ctrls)
{
    if(vector_size(context->module->mems) < 1) {
        return -1;
    }
    uint32_t maxAlign = 0;
    ValueType resultType;
    switch(instr->parent.opcode) {
        case Op_i32_store:
            maxAlign = 2;
            resultType = Value_i32;
            break;
        case Op_i64_store:
            maxAlign = 3;
            resultType = Value_i64;
            break;
        case Op_f32_store:
            maxAlign = 2;
            resultType = Value_f32;
            break;
        case Op_f64_store:
            maxAlign = 3;
            resultType = Value_f64;
            break;
        default:
            return -2;
    }
    if(instr->align > maxAlign) {
        return -3;
    }
    ValueType* operand = NULL;
    if(pop_opd_expect(opds, ctrls, &operand, resultType)) {
        return -4;
    }
    free(operand);
    ValueType* address = NULL;
    if(pop_opd_expect(opds, ctrls, &address, Value_i32)) {
        return -5;
    }
    free(address);
    return 0;
}
int validate_Instr_storeN(WasmMemoryInstr* instr, Context* context, stack_p opds, stack_p ctrls)
{
    if(vector_size(context->module->mems) < 1) {
        return -1;
    }
    uint32_t maxAlign = 0;
    ValueType resultType;
    switch(instr->parent.opcode) {
        case Op_i32_store8:
            maxAlign = 0;
            resultType = Value_i32;
            break;
        case Op_i32_store16:
            maxAlign = 1;
            resultType = Value_i32;
            break;
        case Op_i64_store8:
            maxAlign = 0;
            resultType = Value_i64;
            break;
        case Op_i64_store16:
            maxAlign = 1;
            resultType = Value_i64;
            break;
        case Op_i64_store32:
            maxAlign = 2;
            resultType = Value_i64;
            break;
        default:
            return -2;
    }
    if(instr->align > maxAlign) {
        return -3;
    }
    ValueType* operand = NULL;
    if(pop_opd_expect(opds, ctrls, &operand, resultType)) {
        return -4;
    }
    free(operand);
    ValueType* address = NULL;
    if(pop_opd_expect(opds, ctrls, &address, Value_i32)) {
        return -5;
    }
    free(address);
    return 0;
}
int validate_Instr_memory_size(WasmMemoryInstr* instr, Context* context, stack_p opds, stack_p ctrls)
{
    if(vector_size(context->module->mems) < 1) {
        return -1;
    }
    ValueType* operand = (ValueType*) malloc(sizeof(ValueType));
    *operand = Value_i32;
    stack_push(opds, operand);
    return 0;
}
int validate_Instr_memory_grow(WasmMemoryInstr* instr, Context* context, stack_p opds, stack_p ctrls)
{
    if(vector_size(context->module->mems) < 1) {
        return -1;
    }
    ValueType* operand = NULL;
    if(pop_opd_expect(opds, ctrls, &operand, Value_i32)) {
        return -2;
    }
    stack_push(opds, operand);
    return 0;
}
int validate_Instr_nop(WasmControlInstr* instr, Context* context, stack_p opds, stack_p ctrls)
{
    return 0;
}
int validate_Instr_unreachable(WasmControlInstr* instr, Context* context, stack_p opds, stack_p ctrls)
{
    // TODO: unreachable
    return 0;
}
int validate_Instr_block(WasmControlInstr* instr, Context* context, stack_p opds, stack_p ctrls)
{
    ctrl_frame* frame = new_ctrl_frame(opds);
    for(size_t i = 0; i < vector_size(instr->resultTypes); ++i) {
        vector_push_back(frame->label_types, vector_at(ValueType*, instr->resultTypes, i));
        vector_push_back(frame->end_types, vector_at(ValueType*, instr->resultTypes, i));
    }
    stack_push(ctrls, frame);
    return 0;
}
int validate_Instr_loop(WasmControlInstr* instr, Context* context, stack_p opds, stack_p ctrls)
{
    ctrl_frame* frame = new_ctrl_frame(opds);
    for(size_t i = 0; i < vector_size(instr->resultTypes); ++i) {
        vector_push_back(frame->end_types, vector_at(ValueType*, instr->resultTypes, i));
    }
    stack_push(ctrls, frame);
    return 0;
}
int validate_Instr_if(WasmControlInstr* instr, Context* context, stack_p opds, stack_p ctrls)
{
    ValueType* operand = NULL;
    if(pop_opd_expect(opds, ctrls, &operand, Value_i32)) {
        return -1;
    }
    ctrl_frame* frame = new_ctrl_frame(opds);
    for(size_t i = 0; i < vector_size(instr->resultTypes); ++i) {
        vector_push_back(frame->label_types, vector_at(ValueType*, instr->resultTypes, i));
        vector_push_back(frame->end_types, vector_at(ValueType*, instr->resultTypes, i));
    }
    stack_push(ctrls, frame);
    free(operand);
    return 0;
}
int validate_Instr_end(WasmControlInstr* instr, Context* context, stack_p opds, stack_p ctrls)
{
    ctrl_frame* frame = stack_top(ctrl_frame*, ctrls);
    if(!frame) {
        return -1;
    }
    for(size_t i = vector_size(frame->end_types); i > 0; --i) {
        size_t index = i - 1;
        ValueType* operand = NULL;
        ValueType* endType = vector_at(ValueType*, frame->end_types, index);
        if(pop_opd_expect(opds, ctrls, &operand, *endType)) {
            free(operand);
            frame = stack_pop(ctrl_frame*, ctrls);
            free_ctrl_frame(frame);
            return -2;
        }
        free(operand);
    }
    if(stack_size(opds) != vector_size(frame->label_types)) {
        return -3;
    }
    for(size_t i = 0; i < vector_size(frame->end_types); ++i) {
        ValueType* operand = (ValueType*) malloc(sizeof(ValueType));
        *operand = *(vector_at(ValueType*, frame->end_types, i));
        stack_push(opds, operand);
    }
    frame = stack_pop(ctrl_frame*, ctrls);
    free_ctrl_frame(frame);
    return 0;
}
int validate_Instr_else(WasmControlInstr* instr, Context* context, stack_p opds, stack_p ctrls)
{
    ctrl_frame* frame = stack_top(ctrl_frame*, ctrls);
    if(!frame) {
        return -1;
    }
    for(size_t i = vector_size(frame->end_types); i > 0; --i) {
        size_t index = i - 1;
        ValueType* operand = NULL;
        ValueType* endType = vector_at(ValueType*, frame->end_types, index);
        if(pop_opd_expect(opds, ctrls, &operand, *endType)) {
            free(operand);
            frame = stack_pop(ctrl_frame*, ctrls);
            free_ctrl_frame(frame);
            return -2;
        }
        free(operand);
    }
    if(stack_size(opds) != vector_size(frame->label_types)) {
        return -3;
    }
    return 0;
}
int validate_Instr_br(WasmControlInstr* instr, Context* context, stack_p opds, stack_p ctrls)
{
    uint32_t index = *vector_at(uint32_t*, instr->indices, 0);
    ctrl_frame* frame = ctrl_at(ctrls, index);
    if(frame == NULL) {
        return -1;
    }
    for(size_t i = vector_size(frame->label_types); i > 0; --i) {
        ValueType* operand = NULL;
        ValueType* expect = vector_at(ValueType*, frame->label_types, i - 1);
        if(pop_opd_expect(opds, ctrls, &operand, *expect)) {
            free(operand);
            return -2;
        }
        free(operand);
    }
    frame = stack_top(ctrl_frame*, ctrls);
    frame->unreachable = 1;
    return 0;
}
int validate_Instr_br_if(WasmControlInstr* instr, Context* context, stack_p opds, stack_p ctrls)
{
    uint32_t index = *vector_at(uint32_t*, instr->indices, 0);
    ctrl_frame* frame = ctrl_at(ctrls, index);
    if(frame == NULL) {
        return -1;
    }
    ValueType* condition = NULL;
    if(pop_opd_expect(opds, ctrls, &condition, Value_i32)) {
        return -2;
    }
    free(condition);
    stack_p operandCache = new_stack_p(free);
    for(size_t i = vector_size(frame->label_types); i > 0; --i) {
        ValueType* operand = NULL;
        ValueType* expect = vector_at(ValueType*, frame->label_types, i - 1);
        if(pop_opd_expect(opds, ctrls, &operand, *expect)) {
            free(operand);
            free_stack_p(operandCache);
            return -3;
        }
        stack_push(operandCache, operand);
    }
    for(ValueType* operand = NULL; stack_size(operandCache) > 0; operand = NULL) {
        operand = stack_pop(ValueType*, operandCache);
        stack_push(opds, operand);
    }
    free_stack_p(operandCache);
    return 0;
}
int validate_Instr_br_table(WasmControlInstr* instr, Context* context, stack_p opds, stack_p ctrls)
{
    uint32_t index = *vector_at(uint32_t*, instr->indices, vector_size(instr->indices) - 1);
    ctrl_frame* frame = ctrl_at(ctrls, index);
    if(frame == NULL) {
        return -1;
    }
    for(size_t i = 0; i < vector_size(instr->indices) - 1; ++i) {
        uint32_t index_n = *vector_at(uint32_t*, instr->indices, i);
        if(stack_size(ctrls) <= index_n) {
            return -2;
        }
        ctrl_frame* frame_n = ctrl_at(ctrls, index_n);
        if(vector_size(frame->label_types) != vector_size(frame_n->label_types)) {
            return -3;
        }
        for(size_t j = vector_size(frame->label_types); j > 0; --j) {
            if(*vector_at(ValueType*, frame->label_types, j - 1) != *vector_at(ValueType*, frame_n->label_types, j - 1)) {
                return -4;
            }
        }
    }
    ValueType* condition = NULL;
    if(pop_opd_expect(opds, ctrls, &condition, Value_i32)) {
        free(condition);
        return -5;
    }
    free(condition);
    for(size_t i = vector_size(frame->label_types); i > 0; --i) {
        ValueType* operand = NULL;
        ValueType* expect = vector_at(ValueType*, frame->label_types, i - 1);
        if(pop_opd_expect(opds, ctrls, &operand, *expect)) {
            free(operand);
            return -6;
        }
        free(operand);
    }
    frame = stack_top(ctrl_frame*, ctrls);
    frame->unreachable = 1;
    return 0;
}
int validate_Instr_return(WasmControlInstr* instr, Context* context, stack_p opds, stack_p ctrls)
{
    return 0; // Expression not inside func was evaluated
}
int validate_Instr_call(WasmControlInstr* instr, Context* context, stack_p opds, stack_p ctrls)
{
    uint32_t index = *vector_at(uint32_t*, instr->indices, 0);
    vector_p funcs = context->module->funcs;
    if(index >= vector_size(funcs)) {
        return -1;
    }
    WasmFunc* func = vector_at(WasmFunc*, context->module->funcs, index);
    // Parameter
    FuncType type = vector_at(FuncType, context->module->types, func->type);
    for(uint32_t i = vector_size(type->params); i > 0; --i) {
        ValueType* param = NULL;
        if(pop_opd_expect(opds, ctrls, &param, *vector_at(ValueType*, type->params, i - 1))) {
            free(param);
            return -2;
        }
        free(param);
    }
    // Push results
    for(uint32_t i = 0; i < vector_size(type->results); ++i) {
        ValueType* result = (ValueType*)malloc(sizeof(ValueType));
        *result = *vector_at(ValueType*, type->results, i);
        stack_push(opds, result);
    }
    return 0;
}
int validate_Instr_call_indirect(WasmControlInstr* instr, Context* context, stack_p opds, stack_p ctrls)
{
    vector_p tables = context->module->tables;
    if(vector_size(tables) <= 0) {
        return -1;
    }
    vector_p types = context->module->types;
    uint32_t index = *vector_at(uint32_t*, instr->indices, 0);
    if(index >= vector_size(types)) {
        return -2;
    }
    // Func parameter
    FuncType type = vector_at(FuncType, types, index);
    for(uint32_t i = vector_size(type->params); i > 0; --i) {
        ValueType* param = NULL;
        if(pop_opd_expect(opds, ctrls, &param, *vector_at(ValueType*, type->params, i - 1))) {
            free(param);
            return -3;
        }
        free(param);
    }
    // Operand
    ValueType* operand = NULL;
    if(pop_opd_expect(opds, ctrls, &operand, Value_i32)) {
        free(operand);
        return -4;
    }
    free(operand);
    // Push results
    for(uint32_t i = 0; i < vector_size(type->results); ++i) {
        ValueType* result = (ValueType*)malloc(sizeof(ValueType));
        *result = *vector_at(ValueType*, type->results, i);
        stack_push(opds, result);
    }
    return 0;
}
