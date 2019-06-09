#ifndef WASMVM_LOADERREQUEST_DEF
#define WASMVM_LOADERREQUEST_DEF

#include <Request.h>
#include <core/Store.h>
#include <Executor.h>
#include <dataTypes/vector_p.h>
#include <Component.h>

typedef struct LoaderRequest_ {
    Request parent;
    char* moduleName;
} LoaderRequest;

LoaderRequest* new_LoaderRequest(const char* moduleName, Component* loader, Executor* executor);
void free_LoaderRequest(LoaderRequest* request);

#endif