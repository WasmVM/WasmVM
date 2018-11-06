#ifndef WASMVM_STRUCTURE_MODULE
#define WASMVM_STRUCTURE_MODULE

#include <stdint.h>
#include <dataTypes/vector.h>
#include <dataTypes/FuncType.h>
#include <structures/WasmData.h>
#include <structures/WasmElem.h>
#include <structures/WasmExport.h>
#include <structures/WasmFunc.h>
#include <structures/WasmGlobal.h>
#include <structures/WasmImport.h>
#include <structures/instrs/WasmInstr.h>
#include <structures/WasmMemory.h>
#include <structures/WasmTable.h>

typedef struct _wasm_module {
    char*     module_name;// module_name: string, indicate name of module (aim to support import/export)
    vector*   types;      // types:   Vector<FuncType>
    vector*   funcs;      // funcs:   Vector<WasmFunc>
    vector*   tables;     // tables:  Vector<WasmTable>
    vector*   mems;       // mems:    Vector<WasmMemory>
    vector*   globals;    // globals: Vector<WasmGlobal>
    vector*   elems;      // globals: Vector<WasmElem>
    vector*   datas;      // globals: Vector<WasmData>
    uint32_t  start;      // start:   uint32_t
    vector*   imports;    // imports: Vector<WasmImport>
    vector*   exports;    // exports: Vector<WasmExport>
} WasmModule;

WasmModule* new_WasmModule();
void free_WasmModule(WasmModule* module);

#endif