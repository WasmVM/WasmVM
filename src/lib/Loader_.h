#ifndef WASMVM_LOADER__DEF
#define WASMVM_LOADER__DEF

#include <Component.h>
#include <dataTypes/list_p.h>
#include <dataTypes/stack_p.h>
#include <dataTypes/queue_p.h>
#include <Loader.h>

struct Loader_ {
    Component parent;
    list_p loadedList; // char*
    stack_p decodedStack; // Request*
    queue_p requests; // Request*
};

#endif