#ifndef WASMVM_LOADER_DEF
#define WASMVM_LOADER_DEF

#include <Component.h>
#include <Request.h>
#include <dataTypes/list.h>
#include <dataTypes/stack.h>
#include <dataTypes/queue.h>

typedef struct Loader_{
    Component parent;
    list* loadedList; // char*
    stack* decodedStack; // Request*
    queue* requests; // Request*
    void (*addRequest)(struct Loader_* loader, Request* request);
} Loader;

Loader* new_Loader();
void free_Loader(Loader* loader);

#endif