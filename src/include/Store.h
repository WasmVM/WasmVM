#ifndef WASMVM_STORE
#define WASMVM_STORE

#include <dataTypes/vector_t.h>
#include <instance/FuncInst.h>
#include <instance/TableInst.h>
#include <instance/MemInst.h>
#include <instance/GlobalInst.h>

typedef struct {
    vector_t(FuncInst) funcs;
    vector_t(TableInst) tables;
    vector_t(MemInst) mems;
    vector_t(GlobalInst) globals;
} Store;

typedef Store* wasm_store;

#endif