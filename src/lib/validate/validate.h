#ifndef WASMVM_VALIDATE_VALIDATE
#define WASMVM_VALIDATE_VALIDATE

#include <WasmVM.h>

typedef struct {
    vector_t(u32_t) funcs;
    vector_t(WasmTable) tables;
    vector_t(WasmMemory) mems;
    vector_t(ValueType) globals;
} ValidateContext;


_Bool func_validate(WasmFunc* func, WasmModule* module, ValidateContext* context);
_Bool table_validate(WasmTable* table);
_Bool memory_validate(WasmMemory* memory);
_Bool global_validate(WasmGlobal* global, WasmModule* module, ValidateContext* context);
_Bool elem_validate(WasmElem* elem, WasmModule* module, ValidateContext* context);
_Bool data_validate(WasmData* data, WasmModule* module, ValidateContext* context);
_Bool import_validate(WasmImport* import, WasmModule* module, ValidateContext* context);
_Bool export_validate(WasmExport* export, WasmModule* module, ValidateContext* context);

#endif