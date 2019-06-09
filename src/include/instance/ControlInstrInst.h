#ifndef WASMVM_INSTANCE_CONTROLINSTRINST_DEF
#define WASMVM_INSTANCE_CONTROLINSTRINST_DEF

#include <stdint.h>
#include <dataTypes/vector_p.h>
#include <dataTypes/Value.h>
#include <instance/InstrInst.h>

typedef struct {
    InstrInst parent;
    vector_p resultTypes; // ValueType
    uint32_t elseAddr;
    uint32_t endAddr;
    vector_p indices; // uint32_t
} ControlInstrInst;

ControlInstrInst* new_ControlInstrInst();
void clean_ControlInstrInst(ControlInstrInst* instance);
void free_ControlInstrInst(ControlInstrInst* instance);
#endif
