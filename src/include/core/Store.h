#ifndef WASMVM_CORE_STORE_DEF
#define WASMVM_CORE_STORE_DEF

#include <stdlib.h>
#include <dataTypes/vector.h>
#include <instance/FuncInst.h>
#include <instance/TableInst.h>
#include <instance/MemInst.h>
#include <instance/GlobalInst.h>

typedef struct {
    vector *funcs;      // FuncInst
    vector *tables;     // TableInst
    vector *mems;       // MemInst
    vector *globals;    // GlobalInst
} Store;

Store* new_Store();
void free_Store(Store* thisStore);

#endif