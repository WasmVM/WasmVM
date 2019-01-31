#ifndef WASMVM_INSTANCE_TABLEINST_DEF
#define WASMVM_INSTANCE_TABLEINST_DEF

#include <stdint.h>
#include <dataTypes/vector.h>

typedef struct {
    vector* elem; // uint32_t
    uint32_t max;
} TableInst;

TableInst* new_TableInst();
void clean_TableInst(TableInst* tableInst);
void free_TableInst(TableInst* tableInst);

#endif