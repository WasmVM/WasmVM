#ifndef WASMVM_STRUCTURE_INSTR_CONTROL
#define WASMVM_STRUCTURE_INSTR_CONTROL

#include <stdint.h>
#include <dataTypes/Value.h>
#include <dataTypes/vector.h>
#include <structures/instrs/WasmInstr.h>

typedef struct _wasm_control_instr {
    WasmInstr   parent;
    vector*     resultTypes;    // ValueType
    vector*     instrs;         // WasmInstr
    vector*     indices;        // uint32_t
} WasmControlInstr;

WasmControlInstr* new_WasmControlInstr();
void free_WasmControlInstr(WasmControlInstr* thisInstr);
#endif
