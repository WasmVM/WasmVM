#include "vector_p_.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void free_vector_p(vector_p vectorPtr)
{
    if(vectorPtr == NULL) {
        return;
    }
    if(vectorPtr->cleanFunc) {
        for(uint32_t i = vectorPtr->length; i > 0; --i) {
            vectorPtr->cleanFunc((void*)((char*)vectorPtr->data + (i - 1) * vectorPtr->unitSize));
        }
    }
    free(vectorPtr->data);
    free(vectorPtr);
}
void vector_shrink(vector_p thisVector)
{
    if(thisVector->length < thisVector->capacity) {
        thisVector->data = realloc(thisVector->data, thisVector->unitSize * thisVector->length);
        thisVector->capacity = thisVector->length;
    }
}
void vector_resize(vector_p thisVector, size_t length)
{
    if(length < thisVector->length && thisVector->cleanFunc) {
        for(size_t i = thisVector->length - length; i > 0; --i) {
            thisVector->cleanFunc(thisVector->data + (thisVector->unitSize * (length + i - 1)));
        }
    }
    if(length != thisVector->length) {
        thisVector->length = length;
        thisVector->capacity = length;
        thisVector->data = realloc(thisVector->data, thisVector->unitSize * thisVector->length);
    }
}

size_t vector_size(vector_p thisVector)
{
    return thisVector->length;
}

vector_p new_vector_p_(size_t unitSize, void (*cleanFunc)(void*))
{
    vector_p thisVector = (vector_p) malloc(sizeof(struct vector_p_));
    thisVector->data = NULL;
    thisVector->length = 0;
    thisVector->capacity = 0;
    thisVector->cleanFunc = cleanFunc;
    thisVector->unitSize = unitSize;
    return thisVector;
}
void vector_push_back_(vector_p thisVector, const void* valuePtr)
{
    if(thisVector->length >= thisVector->capacity) {
        if(thisVector->length == 0) {
            thisVector->data = malloc(thisVector->unitSize);
            thisVector->capacity = 1;
        } else {
            thisVector->data = realloc(thisVector->data, thisVector->unitSize * thisVector->length * 2);
            thisVector->capacity = thisVector->length * 2;
        }
    }
    memcpy(thisVector->data + (thisVector->unitSize * thisVector->length), valuePtr, thisVector->unitSize);
    ++thisVector->length;
}
void* vector_pop_back_(vector_p thisVector)
{
    if(thisVector->length <= 0) {
        return NULL;
    }
    void* valuePtr = malloc(thisVector->unitSize);
    memcpy(valuePtr, vector_at(void*, thisVector, thisVector->length - 1), thisVector->unitSize);
    --thisVector->length;
    if(thisVector->cleanFunc) {
        thisVector->cleanFunc(thisVector->data + (thisVector->unitSize * thisVector->length));
    }
    return valuePtr;
}
void* vector_at_(vector_p thisVector, size_t index)
{
    if(thisVector->length <= 0 || index >= thisVector->length) {
        return NULL;
    }
    return (void*)(((uint8_t*)thisVector->data) + (index * thisVector->unitSize));
}
void* vector_data_(vector_p thisVector)
{
    return thisVector->data;
}
