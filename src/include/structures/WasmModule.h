#ifndef WASMVM_STRUCTURE_MODULE
#define WASMVM_STRUCTURE_MODULE

#include <dataTypes/vector_t.h>
#include <defines.h>
#include <dataTypes/FuncType.h>
#include <structures/WasmImport.h>
#include <structures/WasmFunc.h>
// #include <structures/WasmData.h>
// #include <structures/WasmElem.h>
// #include <structures/WasmExport.h>
// #include <structures/WasmGlobal.h>
// #include <structures/instrs/WasmInstr.h>
// #include <structures/WasmMemory.h>
// #include <structures/WasmTable.h>

typedef struct module_t {
    // char*          modulename; // module_name: string, indicate name of module (aim to support import/export)
    vector_t(FuncType)   types;   // types:   Vector<FuncType>
    vector_t(WasmImport) imports; // imports: Vector<WasmImport>
    vector_t(WasmFunc)   funcs;   // funcs:   Vector<WasmFunc>
    // vector_p   tables;     // tables:  Vector<WasmTable>
    // vector_p   mems;       // mems:    Vector<WasmMemory>
    // vector_p   globals;    // globals: Vector<WasmGlobal>
    // vector_p   elems;      // elems: Vector<WasmElem>
    // vector_p   datas;      // datas: Vector<WasmData>
    u32_t   start;                  // start:   u32_t
    // vector_p   exports;    // exports: Vector<WasmExport>
} WasmModule;

// WasmModule* new_WasmModule(char* module_name);
// void clean_WasmModule(WasmModule* module);
// void free_WasmModule(WasmModule* module);

#endif