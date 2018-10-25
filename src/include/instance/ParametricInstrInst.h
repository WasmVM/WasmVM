#ifndef WASMVM_INSTANCE_PARAMETRICINSTRINST_DEF
#define WASMVM_INSTANCE_PARAMETRICINSTRINST_DEF

#include <instance/InstrInst.h>

typedef InstrInst ParametricInstrInst;

ParametricInstrInst* new_ParametricInstrInst();
void free_ParametricInstrInst(ParametricInstrInst* instrInst);

#endif