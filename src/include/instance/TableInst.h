#ifndef WASMVM_INSTANCE_TABLEINST_DEF
#define WASMVM_INSTANCE_TABLEINST_DEF

#include <stdint.h>
#include <stdlib.h>
#include <dataTypes/vector.h>

typedef struct {
    vector* elem; // uint32_t
    uint32_t max;
} TableInst;

TableInst* new_TableInst();
void free_TableInst(TableInst* tableInst);

#endif