#ifndef WASMVM_STRUCTURE_FUNC
#define WASMVM_STRUCTURE_FUNC

#include <stdint.h>
#include <dataTypes/Value.h>
#include <dataTypes/list_p.h>
#include <dataTypes/vector_p.h>
#include <structures/instrs/WasmInstr.h>

typedef struct _wasm_func {
    uint32_t    type;
    vector_p     locals;     // ValueType
    list_p        body;       // WasmInstr
} WasmFunc;
WasmFunc* new_WasmFunc();
void clean_WasmFunc(WasmFunc* thisWasmFunc);
void free_WasmFunc(WasmFunc* thisWasmFunc);

#endif