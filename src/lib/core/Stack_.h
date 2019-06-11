#ifndef WASMVM_CORE_STACK__DEF
#define WASMVM_CORE_STACK__DEF

#include <core/Stack.h>
#include <dataTypes/stack_p.h>

struct Stack_ {
    stack_p entries;
    Label curLabel;
    Frame curFrame;
};

#endif