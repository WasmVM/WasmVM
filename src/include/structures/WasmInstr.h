#ifndef WASMVM_STRUCTURE_Instr
#define WASMVM_STRUCTURE_Instr

#include <defines.h>
#include <dataTypes/Value.h>
#include <dataTypes/vector_t.h>

typedef struct {
    u8_t opcode;
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

#define get_imm_vector(T, V) (vector_t(T)*)(&(V.imm.vec))

// void clean_WasmInstr(WasmInstr* instr);
// void free_WasmInstr(WasmInstr* instr);

#endif