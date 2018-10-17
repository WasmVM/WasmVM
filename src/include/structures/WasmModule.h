#ifndef WASMVM_STRUCTURE_MODULE
#define WASMVM_STRUCTURE_MODULE

#include <stdint.h>
#include <dataTypes/FuncType.h>
#include <structures/WasmData.h>
#include <structures/WasmElem.h>
#include <structures/WasmExport.h>
#include <structures/WasmFunc.h>
#include <structures/WasmGlobal.h>
#include <structures/WasmImport.h>
#include <structures/WasmInstr.h>
#include <structures/WasmMemory.h>
#include <structures/WasmTable.h>

typedef struct _wasm_module {
    FuncType*   types;      // types:   Vector<FuncType>
    WasmFunc*   funcs;      // funcs:   Vector<WasmFunc>
    WasmTable*  tables;     // tables:  Vector<WasmTable>
    WasmMemory* mems;       // mems:    Vector<WasmMemory>
    WasmGlobal* globals;    // globals: Vector<WasmGlobal>
    WasmElem*   elems;      // globals: Vector<WasmElem>
    WasmData*   datas;      // globals: Vector<WasmData>
    uint32_t    start;      // start:   uint32_t
    WasmImport* imports;    // imports: Vector<WasmImport>
    WasmExport* exports;    // exports: Vector<WasmExport>
} WasmModule;

#endif