#ifndef WASMVM_INSTANCE_TABLEINST_DEF
#define WASMVM_INSTANCE_TABLEINST_DEF

#include <stdint.h>
#include <dataTypes/vector_p.h>

typedef struct {
    vector_p elem; // uint32_t
    uint32_t max;
} TableInst;

TableInst* new_TableInst();
void clean_TableInst(TableInst* tableInst);
void free_TableInst(TableInst* tableInst);

#endif