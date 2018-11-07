#ifndef WASMVM_INSTANCE_NUMEFICINSTRINST_DEF
#define WASMVM_INSTANCE_NUMEFICINSTRINST_DEF

#include <dataTypes/Value.h>
#include <instance/InstrInst.h>

typedef struct {
    InstrInst parent;
    Value constant;
} NumericInstrInst;

NumericInstrInst* new_NumericInstrInst();
void free_NumericInstrInst(NumericInstrInst* instrInst);

#endif