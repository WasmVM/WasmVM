#ifndef WASMVM_LOADER_DEF
#define WASMVM_LOADER_DEF

#include <Component.h>
#include <LoaderRequest.h>
#include <dataTypes/list_p.h>
#include <dataTypes/stack_p.h>
#include <dataTypes/queue_p.h>

typedef struct Loader_ {
    Component parent;
    list_p loadedList; // char*
    stack* decodedStack; // Request*
    queue_p requests; // Request*
    void (*addRequest)(struct Loader_* loader, LoaderRequest* request);
} Loader;

Loader* new_Loader();
void free_Loader(Loader* loader);

#endif