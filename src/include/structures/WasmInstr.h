#ifndef WASMVM_STRUCTURE_Instr
#define WASMVM_STRUCTURE_Instr

#include <dataTypes/Value.h>

typedef struct _wasm_instr {
    uint8_t opcode;
} WasmInstr;

typedef struct _wasm_numeric_instr {
    uint8_t opcode;
    Value   constant;
} WasmNumericInstr;

typedef struct _wasm_parametric_instr {
    uint8_t opcode;
} WasmParametricInstr;

typedef struct _wasm_variable_instr {
    uint32_t    index;
    uint8_t     opcode;
} WasmVariableInstr;

typedef struct _wasm_memory_instr {
    uint8_t     opcode;
    uint32_t    offset;
    uint32_t    align;
} WasmMemoryInstr;

typedef struct _wasm_control_instr {
    uint8_t     opcode;
    ValueType   resulttype;
    WasmInstr*  instrs;
    uint32_t*   indices;
} WasmControlInstr;

#endif