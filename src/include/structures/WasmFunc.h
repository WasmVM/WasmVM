#ifndef WASMVM_STRUCTURE_FUNC
#define WASMVM_STRUCTURE_FUNC

#include <dataTypes/Value.h>
#include <WasmInstr.h>

typedef struct _wasm_func {
    uint32_t    type;
    ValueType*  locals;
    WasmInstr*  body;
} WasmFunc;

#endif