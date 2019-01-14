#include "Validates.h"

#include <dataTypes/stack.h>
#include <Opcodes.h>
#include <stdlib.h>

static int pop_opd(stack* opds, stack* ctrls, ValueType** operand)
{
    ctrl_frame* frame = NULL;
    ctrls->top(ctrls, (void**)&frame);
    if(opds->size == frame->height) {
        if(frame->unreachable) {
            *operand = (ValueType*) malloc(sizeof(ValueType));
            **operand = Value_Unspecified;
            return 0;
        } else {
            return -1;
        }
    }
    opds->pop(opds, (void**)operand);
    return 0;
}

static int pop_opd_expect(stack* opds, stack* ctrls, ValueType** operand, ValueType expect)
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

static ctrl_frame* ctrl_at(stack* ctrls, size_t index)
{
    if(ctrls->size <= index) {
        return NULL;
    }
    stackNode* cursor = ctrls->head;
    for(size_t i = 0; i < index; ++i) {
        cursor = cursor->next;
    }
    return (ctrl_frame*) cursor->data;
}

int validate_Instr_const(WasmNumericInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    ValueType* type = (ValueType*) malloc(sizeof(ValueType));
    *type = instr->constant.type;
    opds->push(opds, type);
    return 0;
}
int validate_Instr_unop(WasmNumericInstr* instr, Context* context, stack* opds, stack* ctrls)
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
    opds->push(opds, operand);
    return 0;
}
int validate_Instr_binop(WasmNumericInstr* instr, Context* context, stack* opds, stack* ctrls)
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
    opds->push(opds, operand1);
    free(operand2);
    return 0;
}
int validate_Instr_testop(WasmNumericInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    return 0;
}
int validate_Instr_relop(WasmNumericInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    return 0;
}
int validate_Instr_cvtop(WasmNumericInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    return 0;
}
int validate_Instr_drop(WasmParametricInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    return 0;
}
int validate_Instr_select(WasmParametricInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    return 0;
}
int validate_Instr_get_local(WasmVariableInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    return 0;
}
int validate_Instr_set_local(WasmVariableInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    return 0;
}
int validate_Instr_tee_local(WasmVariableInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    return 0;
}
int validate_Instr_get_global(WasmVariableInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    return 0;
}
int validate_Instr_set_global(WasmVariableInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    return 0;
}
int validate_Instr_load(WasmMemoryInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    return 0;
}
int validate_Instr_loadN(WasmMemoryInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    return 0;
}
int validate_Instr_store(WasmMemoryInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    return 0;
}
int validate_Instr_storeN(WasmMemoryInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    return 0;
}
int validate_Instr_memory_size(WasmMemoryInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    return 0;
}
int validate_Instr_memory_grow(WasmMemoryInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    return 0;
}
int validate_Instr_nop(WasmControlInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    return 0;
}
int validate_Instr_unreachable(WasmControlInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    // TODO: unreachable
    return 0;
}
int validate_Instr_block(WasmControlInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    ctrl_frame* frame = new_ctrl_frame(opds);
    for(size_t i = 0; i < instr->resultTypes->length; ++i) {
        frame->label_types->push_back(frame->label_types, instr->resultTypes->at(instr->resultTypes, i));
        frame->end_types->push_back(frame->end_types, instr->resultTypes->at(instr->resultTypes, i));
    }
    ctrls->push(ctrls, frame);
    return 0;
}
int validate_Instr_loop(WasmControlInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    ctrl_frame* frame = new_ctrl_frame(opds);
    for(size_t i = 0; i < instr->resultTypes->length; ++i) {
        frame->end_types->push_back(frame->end_types, instr->resultTypes->at(instr->resultTypes, i));
    }
    ctrls->push(ctrls, frame);
    return 0;
}
int validate_Instr_if(WasmControlInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    ValueType* operand = NULL;
    if(pop_opd_expect(opds, ctrls, &operand, Value_i32)) {
        return -1;
    }
    ctrl_frame* frame = new_ctrl_frame(opds);
    for(size_t i = 0; i < instr->resultTypes->length; ++i) {
        frame->label_types->push_back(frame->label_types, instr->resultTypes->at(instr->resultTypes, i));
        frame->end_types->push_back(frame->end_types, instr->resultTypes->at(instr->resultTypes, i));
    }
    ctrls->push(ctrls, frame);
    free(operand);
    return 0;
}
int validate_Instr_end(WasmControlInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    ctrl_frame* frame = NULL;
    if(ctrls->top(ctrls, (void**)&frame)) {
        return -1;
    }
    for(size_t i = frame->end_types->length; i > 0; --i) {
        size_t index = i - 1;
        ValueType* operand = NULL;
        ValueType* endType = frame->end_types->at(frame->end_types, index);
        if(pop_opd_expect(opds, ctrls, &operand, *endType)) {
            free(operand);
            ctrls->pop(ctrls, (void**)&frame);
            free_ctrl_frame(frame);
            return -2;
        }
        free(operand);
    }
    if(opds->size != frame->label_types->length) {
        return -3;
    }
    for(size_t i = 0; i < frame->end_types->length; ++i) {
        ValueType* operand = (ValueType*) malloc(sizeof(ValueType));
        *operand = *((ValueType*)frame->end_types->at(frame->end_types, i));
        opds->push(opds, operand);
    }
    ctrls->pop(ctrls, (void**)&frame);
    free_ctrl_frame(frame);
    return 0;
}
int validate_Instr_else(WasmControlInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    ctrl_frame* frame = NULL;
    if(ctrls->top(ctrls, (void**)&frame)) {
        return -1;
    }
    for(size_t i = frame->end_types->length; i > 0; --i) {
        size_t index = i - 1;
        ValueType* operand = NULL;
        ValueType* endType = frame->end_types->at(frame->end_types, index);
        if(pop_opd_expect(opds, ctrls, &operand, *endType)) {
            free(operand);
            ctrls->pop(ctrls, (void**)&frame);
            free_ctrl_frame(frame);
            return -2;
        }
        free(operand);
    }
    if(opds->size != frame->label_types->length) {
        return -3;
    }
    return 0;
}
int validate_Instr_br(WasmControlInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    uint32_t index = *(uint32_t*)instr->indices->at(instr->indices, 0);
    ctrl_frame* frame = ctrl_at(ctrls, index);
    if(frame == NULL) {
        return -1;
    }
    for(size_t i = frame->label_types->length; i > 0; --i) {
        ValueType* operand = NULL;
        ValueType* expect = (ValueType*)frame->label_types->at(frame->label_types, i - 1);
        if(pop_opd_expect(opds, ctrls, &operand, *expect)) {
            free(operand);
            return -2;
        }
        free(operand);
    }
    ctrls->top(ctrls, (void**)&frame);
    frame->unreachable = 1;
    return 0;
}
int validate_Instr_br_if(WasmControlInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    uint32_t index = *(uint32_t*)instr->indices->at(instr->indices, 0);
    ctrl_frame* frame = ctrl_at(ctrls, index);
    if(frame == NULL) {
        return -1;
    }
    ValueType* condition = NULL;
    if(pop_opd_expect(opds, ctrls, &condition, Value_i32)) {
        return -2;
    }
    free(condition);
    stack* operandCache = new_stack(free);
    for(size_t i = frame->label_types->length; i > 0; --i) {
        ValueType* operand = NULL;
        ValueType* expect = (ValueType*)frame->label_types->at(frame->label_types, i - 1);
        if(pop_opd_expect(opds, ctrls, &operand, *expect)) {
            free(operand);
            free_stack(operandCache);
            return -3;
        }
        operandCache->push(operandCache, operand);
    }
    for(ValueType* operand = NULL; operandCache->size > 0; operand = NULL) {
        operandCache->pop(operandCache, (void**)&operand);
        opds->push(opds, operand);
    }
    ctrls->top(ctrls, (void**)&frame);
    free_stack(operandCache);
    return 0;
}
int validate_Instr_br_table(WasmControlInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    uint32_t index = *(uint32_t*)instr->indices->at(instr->indices, instr->indices->length - 1);
    ctrl_frame* frame = ctrl_at(ctrls, index);
    if(frame == NULL) {
        return -1;
    }
    for(size_t i = 0; i < instr->indices->length - 1; ++i) {
        uint32_t index_n = *(uint32_t*)instr->indices->at(instr->indices, i);
        if(ctrls->size <= index_n) {
            return -2;
        }
        ctrl_frame* frame_n = ctrl_at(ctrls, index_n);
        if(frame->label_types->length != frame_n->label_types->length) {
            return -3;
        }
        for(size_t j = frame->label_types->length; j > 0; --j) {
            if(*(ValueType*)frame->label_types->at(frame->label_types, j - 1) != *(ValueType*)frame_n->label_types->at(frame_n->label_types, j - 1)) {
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
    for(size_t i = frame->label_types->length; i > 0; --i) {
        ValueType* operand = NULL;
        ValueType* expect = (ValueType*)frame->label_types->at(frame->label_types, i - 1);
        if(pop_opd_expect(opds, ctrls, &operand, *expect)) {
            free(operand);
            return -6;
        }
        free(operand);
    }
    ctrls->top(ctrls, (void**)&frame);
    frame->unreachable = 1;
    return 0;
}
int validate_Instr_return(WasmControlInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    return 0; // Expression not inside func was evaluated
}
int validate_Instr_call(WasmControlInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    uint32_t index = *(uint32_t*)instr->indices->at(instr->indices, 0);
    vector* funcs = context->module->funcs;
    if(index >= funcs->length) {
        return -1;
    }
    return 0;
}
int validate_Instr_call_indirect(WasmControlInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    vector* tables = context->module->tables;
    if(tables->length <= 0) {
        return -1;
    }
    vector* types = context->module->types;
    uint32_t index = *(uint32_t*)instr->indices->at(instr->indices, 0);
    if(index >= types->length) {
        return -2;
    }
    ValueType* operand = NULL;
    if(pop_opd_expect(opds, ctrls, &operand, Value_i32)) {
        free(operand);
        return -3;
    }
    free(operand);
    return 0;
}
