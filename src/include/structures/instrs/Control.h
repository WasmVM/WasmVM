#ifndef WASMVM_STRUCTURE_INSTR_CONTROL
#define WASMVM_STRUCTURE_INSTR_CONTROL

#include <stdint.h>
#include <dataTypes/Value.h>
#include <dataTypes/vector_p.h>
#include <structures/instrs/WasmInstr.h>

typedef struct _wasm_control_instr {
    WasmInstr   parent;
    vector_p     resultTypes;    // ValueType
    vector_p     instrs;         // WasmInstr
    vector_p     indices;        // uint32_t
} WasmControlInstr;

WasmControlInstr* new_WasmControlInstr();
void clean_WasmControlInstr(WasmControlInstr* thisInstr);
void free_WasmControlInstr(WasmControlInstr* thisInstr);
#endif
