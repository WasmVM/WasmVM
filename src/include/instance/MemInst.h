#ifndef WASMVM_INSTANCE_MEMINST_DEF
#define WASMVM_INSTANCE_MEMINST_DEF

#include <stdint.h>
#include <dataTypes/vector_p.h>

typedef struct MemInst_ {
    vector_p data; // char
    uint32_t max;
} MemInst;

MemInst* new_MemInst();
void clean_MemInst(MemInst* memInst);
void free_MemInst(MemInst* memInst);

#endif