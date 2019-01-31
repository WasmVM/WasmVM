#ifndef WASMVM_STRUCTURE_Instr
#define WASMVM_STRUCTURE_Instr

#include <stdint.h>

typedef struct _wasm_instr {
    uint8_t opcode;
    void (*free)(struct _wasm_instr* instr);
    void (*clean)(struct _wasm_instr* instr);
} WasmInstr;

void clean_WasmInstr(WasmInstr* instr);
void free_WasmInstr(WasmInstr* instr);

#endif