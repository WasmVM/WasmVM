#ifndef WASMVM_INSTANCE_NUMEFICINSTRINST_DEF
#define WASMVM_INSTANCE_NUMEFICINSTRINST_DEF

#include <stdint.h>
#include <dataTypes/vector.h>
#include <instance/InstrInst.h>

typedef struct {
    InstrInst parent;
    vector* resultTypes; // ValueType
    vector* indices; // uint32_t
} ControlInstrInst;

ControlInstrInst* new_ControlInstrInst();
void clean_ControlInstrInst(ControlInstrInst* instance);
void free_ControlInstrInst(ControlInstrInst* instance);
#endif