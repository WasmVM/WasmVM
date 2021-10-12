#ifndef WASMVM_STRUCTURE_Instr
#define WASMVM_STRUCTURE_Instr

#include <defines.h>
#include <dataTypes/Value.h>
#include <dataTypes/vector_t.h>

typedef struct {
    u16_t opcode;
    union {
        struct {
            Value value;
            i32_t index;
        } values;
        struct {
            u32_t offset;
            u32_t align;
        } memarg;
        struct {
            unsigned int size;
            void* data;
        } vec;
    } imm;
} WasmInstr;

void free_Instr(WasmInstr* instr);

#endif