#include "Context.h"

#include <stdlib.h>

Context* new_Context(WasmModule* module, WasmFunc* func)
{
    Context* context = (Context*) malloc(sizeof(Context));
    context->module = module;
    context->locals = new_vector(sizeof(ValueType), NULL);
    // params
    FuncType funcType = (FuncType) module->types->at(module->types, func->type);
    for(size_t i = 0; i < funcType->params->length; ++i) {
        context->locals->push_back(context->locals, funcType->params->at(funcType->params, i));
    }
    // locals
    for(size_t i = 0; i < func->locals->length; ++i) {
        context->locals->push_back(context->locals, func->locals->at(func->locals, i));
    }
    context->returns = new_vector(sizeof(ValueType), NULL);
    // returns
    for(size_t i = 0; i < funcType->results->length; ++i) {
        context->returns->push_back(context->returns, funcType->results->at(funcType->results, i));
    }
    return context;
}

void free_Context(Context* context)
{
    free_vector(context->locals);
    free_vector(context->returns);
    free(context);
}

ctrl_frame* new_ctrl_frame(stack_p opds)
{
    ctrl_frame* frame = (ctrl_frame*) malloc(sizeof(ctrl_frame));
    frame->unreachable = 0;
    frame->label_types = new_vector(sizeof(ValueType), NULL);
    frame->end_types = new_vector(sizeof(ValueType), NULL);
    frame->height = stack_size(opds);
    return frame;
}

void free_ctrl_frame(ctrl_frame* frame)
{
    free_vector(frame->label_types);
    free_vector(frame->end_types);
    free(frame);
}
