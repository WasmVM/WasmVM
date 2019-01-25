#ifndef WASMVM_STRUCTURE_INSTR_VARIABLE
#define WASMVM_STRUCTURE_INSTR_VARIABLE

#include <stdint.h>
#include <structures/instrs/WasmInstr.h>

typedef struct _wasm_variable_instr {
    WasmInstr   parent;
    uint32_t    index;
} WasmVariableInstr;

WasmVariableInstr* new_WasmVariableInstr(uint32_t index);
void free_WasmVariableInstr(WasmVariableInstr* instr);

#endif