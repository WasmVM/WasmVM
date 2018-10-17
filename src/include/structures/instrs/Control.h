#ifndef WASMVM_STRUCTURE_INSTR_MEMORY
#define WASMVM_STRUCTURE_INSTR_MEMORY

#include <stdint.h>
#include <dataTypes/Value.h>
#include <dataTypes/vector.h>
#include <structures/instrs/WasmInstr.h>

typedef struct _wasm_control_instr {
    WasmInstr   opcode;
    ValueType   resulttype;
    vector*     instrs;         // WasmInstr
    vector*     indices;        // uint32_t
} WasmControlInstr;

#endif