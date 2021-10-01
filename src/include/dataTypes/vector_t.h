#ifndef WASMVM_DATATYPES_VECTOR_T_DEF
#define WASMVM_DATATYPES_VECTOR_T_DEF

#include <defines.h>

#define vector_t(T) struct { \
    unsigned int size; \
    T* data; \
}

#define free_vector(V) free_func(V.data); V.size = 0;

#define vector_resize(V, T, NEW_SIZE) V.data = realloc_func(V.data, sizeof(T) * NEW_SIZE);

#define vector_init(V) V.size = 0; V.data = NULL;

#endif