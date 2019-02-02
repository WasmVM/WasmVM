#ifndef WASMVM_CORE_STORE_DEF
#define WASMVM_CORE_STORE_DEF

#include <dataTypes/vector.h>

typedef struct {
    vector *funcs;      // FuncInst
    vector *tables;     // TableInst
    vector *mems;       // MemInst
    vector *globals;    // GlobalInst
} Store;

Store* new_Store();
void clean_Store(Store* thisStore);
void free_Store(Store* thisStore);

#endif