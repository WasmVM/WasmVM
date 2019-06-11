#ifndef WASMVM_CORE_STORE_DEF
#define WASMVM_CORE_STORE_DEF

#include <dataTypes/vector_p.h>

struct Store_ {
    vector_p funcs;      // FuncInst
    vector_p tables;     // TableInst
    vector_p mems;       // MemInst
    vector_p globals;    // GlobalInst
};

typedef struct Store_ * Store;

Store new_Store();
void clean_Store(Store thisStore);
void free_Store(Store thisStore);

#endif