#ifndef WASMVM_STRUCTURE_INSTR_NUMERIC
#define WASMVM_STRUCTURE_INSTR_NUMERIC

#include <dataTypes/Value.h>
#include <structures/instrs/WasmInstr.h>

typedef struct _wasm_numeric_instr {
    WasmInstr   parent;
    Value       constant;
} WasmNumericInstr;

#endif