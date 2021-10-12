#ifndef WASMVM_STORE
#define WASMVM_STORE

#include <dataTypes/vector_t.h>
#include <instance/FuncInst.h>
#include <instance/TableInst.h>

typedef struct {
    vector_t(FuncInst) funcs;      // FuncInst
    vector_t(TableInst) tables;     // TableInst
    // vector_t(FuncInst) mems;       // MemInst
    // vector_t(FuncInst) globals;    // GlobalInst
} Store;

typedef Store* wasm_store;

#endif