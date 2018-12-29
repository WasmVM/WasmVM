#include "Validates.h"

#include <stdlib.h>

static int pop_opd(stack* opds, stack* ctrls, ValueType** operand)
{
    ctrl_frame* frame = NULL;
    ctrls->top(ctrls, (void**)&frame);
    if(opds->size == frame->label_types->length) {
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
    ctrl_frame* frame = new_ctrl_frame();
    for(stackNode* cur = opds->head; cur != NULL; cur = cur->next) {
        frame->label_types->push_back(frame->label_types, cur->data);
    }
    for(size_t i = 0; i < instr->resultTypes->length; ++i) {
        frame->end_types->push_back(frame->end_types, instr->resultTypes->at(instr->resultTypes, i));
    }
    ctrls->push(ctrls, frame);
    return 0;
}
int validate_Instr_loop(WasmControlInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    ctrl_frame* frame = new_ctrl_frame();
    for(stackNode* cur = opds->head; cur != NULL; cur = cur->next) {
        frame->label_types->push_back(frame->label_types, cur->data);
    }
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
    ctrl_frame* frame = new_ctrl_frame();
    for(stackNode* cur = opds->head; cur != NULL; cur = cur->next) {
        frame->label_types->push_back(frame->label_types, cur->data);
    }
    for(size_t i = 0; i < instr->resultTypes->length; ++i) {
        frame->end_types->push_back(frame->end_types, instr->resultTypes->at(instr->resultTypes, i));
    }
    ctrls->push(ctrls, frame);
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
    return 0;
}
int validate_Instr_br(WasmControlInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    return 0;
}
int validate_Instr_br_if(WasmControlInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    return 0;
}
int validate_Instr_br_table(WasmControlInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    return 0;
}
int validate_Instr_return(WasmControlInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    return 0;
}
int validate_Instr_call(WasmControlInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    return 0;
}
int validate_Instr_call_indirect(WasmControlInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    return 0;
}