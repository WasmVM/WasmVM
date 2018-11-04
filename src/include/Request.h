#ifndef WASMVM_REQUEST_DEF
#define WASMVM_REQUEST_DEF

#include <dataTypes/list.h>

typedef struct{
    list* stages; // Stage*
} Request;

#endif