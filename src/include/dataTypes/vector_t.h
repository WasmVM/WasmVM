#ifndef WASMVM_DATATYPES_VECTOR_T_DEF
#define WASMVM_DATATYPES_VECTOR_T_DEF

#include <defines.h>

#define vector_t(T) struct { \
    T* data; \
    unsigned int size; \
}

#define free_vector(V) free_func(V.data)

#define vector_resize(V, T, NEW_SIZE) V.data = realloc_func(V.data, sizeof(T) * NEW_SIZE)

#endif