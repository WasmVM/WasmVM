#ifndef WASMVM_INSTANCE_INSTRINST
#define WASMVM_INSTANCE_INSTRINST

#include <defines.h>

typedef struct {
    u16_t opcode;
    byte_t payload[];
} InstrInst;

#endif