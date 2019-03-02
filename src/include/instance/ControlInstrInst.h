#ifndef WASMVM_INSTANCE_CONTROLINSTRINST_DEF
#define WASMVM_INSTANCE_CONTROLINSTRINST_DEF

#include <stdint.h>
#include <dataTypes/vector.h>
#include <dataTypes/Value.h>
#include <instance/InstrInst.h>

typedef struct {
    InstrInst parent;
    ValueType resultType;
    uint32_t elseAddr;
    uint32_t endAddr;
    vector* indices; // uint32_t
} ControlInstrInst;

ControlInstrInst* new_ControlInstrInst();
void clean_ControlInstrInst(ControlInstrInst* instance);
void free_ControlInstrInst(ControlInstrInst* instance);
#endif
