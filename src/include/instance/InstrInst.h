#ifndef WASMVM_INSTANCE_INSTRINST
#define WASMVM_INSTANCE_INSTRINST

#include <defines.h>
#include <dataTypes/Value.h>
#include <dataTypes/vector_t.h>

typedef struct {
    u16_t opcode;
    byte_t payload[];
} InstrInst;

typedef struct {
    u16_t opcode;
    Value constant;
} ConstInstrInst;

typedef struct {
    u16_t opcode;
    vector_t(ValueType) param;
} SelectInstrInst;

typedef struct {
    u16_t opcode;
    u32_t index;
} UnaryInstrInst;

typedef struct {
    u16_t opcode;
    u32_t index1, index2;
} BinaryInstrInst;

typedef struct {
    u16_t opcode;
    u8_t blocktype;
    u32_t index;
} BlockInstrInst;

typedef struct {
    u16_t opcode;
    vector_t(u32_t) param;
} BrTableInstrInst;

#endif