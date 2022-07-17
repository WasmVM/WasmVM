#ifndef WASMVM_VALIDATE_VALIDATE
#define WASMVM_VALIDATE_VALIDATE

#include <WasmVM.h>

_Bool func_validate(WasmFunc* func, WasmModule* module);
_Bool table_validate(WasmTable* table, WasmModule* module);

#endif