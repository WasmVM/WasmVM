#ifndef WASMVM_VECTOR_DEF
#define WASMVM_VECTOR_DEF

#include <stddef.h>

typedef struct vector_ {
    void *data;
    size_t length;
    size_t capacity;
    size_t unitSize;
    void (*cleanFunc)(void*);
    void (*push_back)(struct vector_* thisVector, const void* value);
    void* (*pop_back)(struct vector_* thisVector);
    void (*shrink)(struct vector_* thisVector);
    void* (*at)(struct vector_* thisVector, size_t index);
} vector;

vector* new_vector(size_t unitSize, void (*cleanFunc)(void*));
void free_vector(vector* vectorPtr);

#endif
