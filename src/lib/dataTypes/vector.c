#include <dataTypes/vector.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static void _push_back(vector* thisVector, const void* valuePtr)
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
static void* _pop_back(vector* thisVector)
{
    if(thisVector->length <= 0) {
        return NULL;
    }
    void* valuePtr = malloc(thisVector->unitSize);
    memcpy(valuePtr, thisVector->at(thisVector, thisVector->length - 1),
           thisVector->unitSize);
    --thisVector->length;
    return valuePtr;
}
static void _shrink(vector* thisVector)
{
    if(thisVector->length < thisVector->capacity) {
        thisVector->data = realloc(thisVector->data, thisVector->unitSize * thisVector->length);
        thisVector->capacity = thisVector->length;
    }
}
static void* _at(vector* thisVector, size_t index)
{
    if(thisVector->length <= 0 || index >= thisVector->length) {
        return NULL;
    }
    return (uint8_t*)thisVector->data + (index * thisVector->unitSize);
}

vector* new_vector(size_t unitSize)
{
    vector* thisVector = (vector*) malloc(sizeof(vector));
    thisVector->data = NULL;
    thisVector->length = 0;
    thisVector->capacity = 0;
    thisVector->unitSize = unitSize;
    thisVector->push_back = _push_back;
    thisVector->pop_back = _pop_back;
    thisVector->shrink = _shrink;
    thisVector->at = _at;
    return thisVector;
}
void free_vector(vector* thisVector)
{
    free(thisVector->data);
    free(thisVector);
}
