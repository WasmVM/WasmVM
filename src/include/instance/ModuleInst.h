#ifndef WASMVM_INSTANCE_MODULEINST_DEF
#define WASMVM_INSTANCE_MODULEINST_DEF

#include <dataTypes/vector.h>

typedef struct ModuleInst_ {
    vector* types;       // FuncType
    vector* funcaddrs;   // uint32_t
    vector* tableaddrs;  // uint32_t
    vector* memaddrs;    // uint32_t
    vector* globaladdrs; // uint32_t
    vector* exports;     // ExportInst
} ModuleInst;

ModuleInst* new_ModuleInst();
void free_ModuleInst(ModuleInst* moduleInst);

#endif