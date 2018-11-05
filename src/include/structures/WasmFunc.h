#ifndef WASMVM_STRUCTURE_FUNC
#define WASMVM_STRUCTURE_FUNC

#include <stdint.h>
#include <dataTypes/Value.h>
#include <dataTypes/vector.h>
#include <structures/instrs/WasmInstr.h>

typedef struct _wasm_func {
    uint32_t    type;
    vector*     locals;     // ValueType
    vector*     body;       // WasmInstr
} WasmFunc;

#endif