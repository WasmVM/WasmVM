#ifndef WASMVM_CORE_STORE_DEF
#define WASMVM_CORE_STORE_DEF

#include <dataTypes/vector_p.h>

typedef struct {
    vector_p funcs;      // FuncInst
    vector_p tables;     // TableInst
    vector_p mems;       // MemInst
    vector_p globals;    // GlobalInst
} Store;

Store* new_Store();
void clean_Store(Store* thisStore);
void free_Store(Store* thisStore);

#endif