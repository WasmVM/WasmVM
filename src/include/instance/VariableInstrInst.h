#ifndef WASMVM_INSTANCE_VARIABLEINSTRINST_DEF
#define WASMVM_INSTANCE_VARIABLEINSTRINST_DEF

#include <stdint.h>
#include <instance/InstrInst.h>

typedef struct {
    InstrInst parent;
    uint32_t index;
} VariableInstrInst;

VariableInstrInst* new_VariableInstrInst();
void free_VariableInstrInst(VariableInstrInst* instrInst);

#endif