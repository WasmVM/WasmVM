#ifndef WASMVM_STRUCTURE_Instr
#define WASMVM_STRUCTURE_Instr

#include <stdint.h>

typedef struct _wasm_instr {
    uint8_t opcode;
} WasmInstr;

#endif