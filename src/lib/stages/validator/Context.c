#include "Context.h"

#include <stdlib.h>

Context* new_Context(WasmModule* module, WasmFunc* func)
{
    Context* context = (Context*) malloc(sizeof(Context));
    context->module = module;
    context->locals = new_vector_p(ValueType, NULL);
    // params
    FuncType funcType = vector_at(FuncType, module->types, func->type);
    for(size_t i = 0; i < vector_size(funcType->params); ++i) {
        vector_push_back(context->locals, vector_at(ValueType*, funcType->params, i));
    }
    // locals
    for(size_t i = 0; i < vector_size(func->locals); ++i) {
        vector_push_back(context->locals, vector_at(ValueType*, func->locals, i));
    }
    context->returns = new_vector_p(ValueType, NULL);
    // returns
    for(size_t i = 0; i < vector_size(funcType->results); ++i) {
        vector_push_back(context->returns, vector_at(ValueType*, funcType->results, i));
    }
    return context;
}

void free_Context(Context* context)
{
    free_vector_p(context->locals);
    free_vector_p(context->returns);
    free(context);
}

ctrl_frame* new_ctrl_frame(stack_p opds)
{
    ctrl_frame* frame = (ctrl_frame*) malloc(sizeof(ctrl_frame));
    frame->unreachable = 0;
    frame->label_types = new_vector_p(ValueType, NULL);
    frame->end_types = new_vector_p(ValueType, NULL);
    frame->height = stack_size(opds);
    return frame;
}

void free_ctrl_frame(ctrl_frame* frame)
{
    free_vector_p(frame->label_types);
    free_vector_p(frame->end_types);
    free(frame);
}
