#ifndef WASMVM_INSTANCE_NUMEFICINSTRINST_DEF
#define WASMVM_INSTANCE_NUMEFICINSTRINST_DEF

#include <stdint.h>
#include <dataTypes/Value.h>
#include <dataTypes/vector.h>
#include <instance/InstrInst.h>

typedef struct {
    InstrInst parent;
    ValueType resultType;
    vector* indices; // uint32_t
} ControlInstrInst;

ControlInstrInst* new_ControlInstrInst();
void free_ControlInstrInst(ControlInstrInst* instance);
#endif