#ifndef WASMVM_DATATYPES_VECTOR_DEF
#define WASMVM_DATATYPES_VECTOR_DEF

#include <stddef.h>

typedef struct vector_p_ * vector_p;

void free_vector_p(vector_p vectorPtr);
void vector_shrink(vector_p thisVector);
void vector_resize(vector_p thisVector, size_t length);
size_t vector_size(vector_p thisVector);

vector_p new_vector_p_(size_t unitSize, void (*cleanFunc)(void*));
void vector_push_back_(vector_p thisVector, const void* valuePtr);
void* vector_pop_back_(vector_p thisVector);
void* vector_at_(vector_p thisVector, size_t index);
void* vector_data_(vector_p thisVector);

#define new_vector_p(T, cleanFunc) new_vector_p_(sizeof(T), (void(*)(void*))cleanFunc)
#define vector_push_back(thisVector, value) vector_push_back_(thisVector, (const void*) value)
#define vector_pop_back(T, thisVector) (T)vector_pop_back_(thisVector)
#define vector_at(T, thisVector, index) (T)vector_at_(thisVector, index)
#define vector_data(T, thisVector) (T)vector_data_(thisVector)

#endif
