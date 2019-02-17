#ifndef WASMVM_INSTANCE_NUMERICINSTRINST_DEF
#define WASMVM_INSTANCE_NUMERICINSTRINST_DEF

#include <dataTypes/Value.h>
#include <instance/InstrInst.h>

typedef struct {
    InstrInst parent;
    Value constant;
} NumericInstrInst;

NumericInstrInst* new_NumericInstrInst();
void free_NumericInstrInst(NumericInstrInst* instrInst);

#endif