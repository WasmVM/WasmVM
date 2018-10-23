#ifndef WASMVM_VALIDATOR_CONTEXT_DEF
#define WASMVM_VALIDATOR_CONTEXT_DEF

#include <stdint.h>
#include <dataTypes/Value.h>
#include <dataTypes/vector.h>
#include <dataTypes/stack.h>

typedef struct {
    vector* label_types; // ValueType
    vector* end_types; // ValueType
    uint32_t height;
    _Bool unreachable;
} ctrl_frame;

typedef struct {
    stack* opd_stack; // ValueType
    stack* ctrl_stack; // ctrl_frame
} Context;

#endif