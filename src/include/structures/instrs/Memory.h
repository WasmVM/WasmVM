#ifndef WASMVM_STRUCTURE_INSTR_MEMORY
#define WASMVM_STRUCTURE_INSTR_MEMORY

#include <stdint.h>
#include <structures/instrs/WasmInstr.h>

typedef struct _wasm_memory_instr {
    WasmInstr   opcode;
    uint32_t    offset;
    uint32_t    align;
} WasmMemoryInstr;

#endif