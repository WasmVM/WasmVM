#ifndef WASMVM_VALIDATOR_CONTEXT_DEF
#define WASMVM_VALIDATOR_CONTEXT_DEF

#include <stdint.h>
#include <dataTypes/Value.h>
#include <dataTypes/vector_p.h>
#include <dataTypes/stack_p.h>
#include <structures/WasmModule.h>
#include <structures/WasmFunc.h>

typedef struct {
    vector_p label_types; // ValueType
    vector_p end_types; // ValueType
    size_t height;
    _Bool unreachable;
} ctrl_frame;

typedef struct {
    WasmModule* module;
    vector_p locals; // ValueType
    vector_p returns; // ValueType
} Context;

ctrl_frame* new_ctrl_frame(stack_p opds);
void free_ctrl_frame(ctrl_frame* frame);
Context* new_Context(WasmModule* module, WasmFunc* func);
void free_Context(Context* context);

#endif