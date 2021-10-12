#ifndef WASMVM_INSTANCE_MODULEINST_DEF
#define WASMVM_INSTANCE_MODULEINST_DEF

#include <dataTypes/vector_t.h>
#include <dataTypes/FuncType.h>

typedef struct {
    vector_t(FuncType) types;       // FuncType
    // vector_p funcaddrs;   // uint32_t
    // vector_p tableaddrs;  // uint32_t
    // vector_p memaddrs;    // uint32_t
    // vector_p globaladdrs; // uint32_t
    // vector_p exports;     // ExportInst
} ModuleInst;

#endif