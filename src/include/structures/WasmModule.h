#ifndef WASMVM_STRUCTURE_MODULE
#define WASMVM_STRUCTURE_MODULE

#include <dataTypes/vector_t.h>
#include <defines.h>
#include <dataTypes/FuncType.h>
#include <structures/WasmImport.h>
#include <structures/WasmFunc.h>
#include <structures/WasmTable.h>
#include <structures/WasmMemory.h>
#include <structures/WasmGlobal.h>
#include <structures/WasmExport.h>
#include <structures/WasmElem.h>
#include <structures/WasmInstr.h>
// #include <structures/WasmData.h>

typedef struct module_t {
    // char*          modulename; // module_name: string, indicate name of module (aim to support import/export)
    vector_t(FuncType)   types;   // types:   Vector<FuncType>
    vector_t(WasmImport) imports; // imports: Vector<WasmImport>
    vector_t(WasmFunc)   funcs;   // funcs:   Vector<WasmFunc>
    vector_t(WasmTable)  tables;  // tables:  Vector<WasmTable>
    vector_t(WasmMemory) mems;    // mems:    Vector<WasmMemory>
    vector_t(WasmGlobal) globals; // globals: Vector<WasmGlobal>
    vector_t(WasmExport) exports; // exports: Vector<WasmExport>
    u32_t   start;                // start:   u32_t
    vector_t(WasmElem)   elems;   // elems: Vector<WasmElem>
    // vector_t(WasmElem)   datas;      // datas: Vector<WasmData>
} WasmModule;

// WasmModule* new_WasmModule(char* module_name);
// void clean_WasmModule(WasmModule* module);
// void free_WasmModule(WasmModule* module);

#endif