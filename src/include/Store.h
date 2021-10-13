#ifndef WASMVM_STORE
#define WASMVM_STORE

#include <dataTypes/vector_t.h>
#include <instance/FuncInst.h>
#include <instance/TableInst.h>
#include <instance/MemInst.h>
#include <instance/GlobalInst.h>
#include <instance/ElemInst.h>
#include <instance/DataInst.h>

typedef struct {
    vector_t(FuncInst) funcs;
    vector_t(TableInst) tables;
    vector_t(MemInst) mems;
    vector_t(GlobalInst) globals;
    vector_t(ElemInst) elems;
    vector_t(DataInst) datas;
} Store;

typedef Store* wasm_store;

#endif