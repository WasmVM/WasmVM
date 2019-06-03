#ifndef WASMVM_STRUCTURE_FUNC
#define WASMVM_STRUCTURE_FUNC

#include <stdint.h>
#include <dataTypes/Value.h>
#include <dataTypes/list.h>
#include <dataTypes/vector.h>
#include <structures/instrs/WasmInstr.h>

typedef struct _wasm_func {
    uint32_t    type;
    vector*     locals;     // ValueType
    list        body;       // WasmInstr
} WasmFunc;
WasmFunc* new_WasmFunc();
void clean_WasmFunc(WasmFunc* thisWasmFunc);
void free_WasmFunc(WasmFunc* thisWasmFunc);

#endif