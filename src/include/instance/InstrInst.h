#ifndef WASMVM_INSTANCE_INSTRINST_DEF
#define WASMVM_INSTANCE_INSTRINST_DEF

#include <stdint.h>

typedef struct InstrInst_{
    uint8_t opcode;
    void (*free)(struct InstrInst_* instrInst);
} InstrInst;

InstrInst* new_InstrInst();
void free_InstrInst(InstrInst* instrInst);

#endif