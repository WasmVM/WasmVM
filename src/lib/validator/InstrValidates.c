#include "Validates.h"

#include <dataTypes/stack.h>
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
            return -1;
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

    return 0;
}
int validate_Instr_unop(WasmNumericInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    return 0;
}
int validate_Instr_binop(WasmNumericInstr* instr, Context* context, stack* opds, stack* ctrls)
{
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
    stack* operandCache = new_stack();
    for(size_t i = frame->label_types->length; i > 0; --i) {
        ValueType* operand = NULL;
        ValueType* expect = (ValueType*)frame->label_types->at(frame->label_types, i - 1);
        if(pop_opd_expect(opds, ctrls, &operand, *expect)) {
            free(operand);
            return -3;
        }
        operandCache->push(operandCache, operand);
    }
    for(ValueType* operand = NULL; operandCache->size > 0; operand = NULL) {
        operandCache->pop(operandCache, (void**)&operand);
        opds->push(opds, operand);
    }
    ctrls->top(ctrls, (void**)&frame);
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