#ifndef WASMVM_INSTANCE_GLOBAL_DEF
#define WASMVM_INSTANCE_GLOBAL_DEF

#include <stdlib.h>
#include <dataTypes/Value.h>

typedef struct {
    Value value;
    _Bool mul;
} GlobalInst;

GlobalInst* new_GlobalInst();
void free_GlobalInst(GlobalInst* globalInst);

#endif