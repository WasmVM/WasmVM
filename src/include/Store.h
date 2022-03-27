#ifndef WASMVM_STORE
#define WASMVM_STORE

#include <dataTypes/vector_t.h>
#include <instances/FuncInst.h>
#include <instances/TableInst.h>
#include <instances/MemInst.h>
#include <instances/GlobalInst.h>
#include <instances/ElemInst.h>
#include <instances/DataInst.h>

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