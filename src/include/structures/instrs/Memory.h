#ifndef WASMVM_STRUCTURE_INSTR_MEMORY
#define WASMVM_STRUCTURE_INSTR_MEMORY

#include <stdint.h>
#include <structures/instrs/WasmInstr.h>

typedef struct _wasm_memory_instr {
    WasmInstr   parent;
    uint32_t    align;
    uint32_t    offset;
} WasmMemoryInstr;

WasmMemoryInstr* new_WasmMemoryInstr(uint32_t align, uint32_t offset);
void free_WasmMemoryInstr(WasmMemoryInstr* instr);

#endif