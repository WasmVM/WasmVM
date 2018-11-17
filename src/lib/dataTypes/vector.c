#include <dataTypes/vector.h>

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
    void* valuePtr = thisVector->at(thisVector, thisVector->length - 1);
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
    void* result = malloc(thisVector->unitSize);
    memcpy(result, thisVector->data + (index * thisVector->unitSize), thisVector->unitSize);
    return result;
}

vector* new_vector(size_t unitSize, void (*freeElem)(void* elem))
{
    vector* thisVector = (vector*) malloc(sizeof(vector));
    thisVector->data = NULL;
    thisVector->length = 0;
    thisVector->capacity = 0;
    thisVector->unitSize = unitSize;
    thisVector->freeElem = freeElem;
    thisVector->push_back = _push_back;
    thisVector->pop_back = _pop_back;
    thisVector->shrink = _shrink;
    thisVector->at = _at;
    return thisVector;
}
void free_vector(vector* thisVector)
{
    if(thisVector->freeElem) {
        for(size_t i = 0; i < thisVector->length; ++i) {
            thisVector->freeElem(thisVector->at(thisVector, i));
        }
    }
    free(thisVector->data);
    free(thisVector);
}