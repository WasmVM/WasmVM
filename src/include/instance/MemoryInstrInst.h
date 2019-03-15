#ifndef WASMVM_INSTANCE_MEMORYINSTRINST_DEF
#define WASMVM_INSTANCE_MEMORYINSTRINST_DEF

#include <stdint.h>
#include <instance/InstrInst.h>

typedef struct {
    InstrInst parent;
    uint32_t offset;
    uint32_t align;
} MemoryInstrInst;

MemoryInstrInst* new_MemoryInstrInst();
void free_MemoryInstrInst(MemoryInstrInst* instrInst);

#endif