#include <Loader.h>

#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stddef.h>
#include <stdio.h>

#include <Stage.h>

static void addRequest(struct Loader_* loader, LoaderRequest* request)
{
    // Check whether loaded or not
    _Bool loaded = 0;
    for(list_iterator it = list_head(loader->loadedList); it != NULL; it = list_next(it)) {
        if(!strcmp(request->moduleName, list_iterator_get(char*, it))) {
            loaded = 1;
            break;
        }
    }
    if(loaded) {
        request->parent.free((Request*)request);
        return;
    }
    char* loadName = (char*)malloc(sizeof(char) * strlen(request->moduleName));
    strcpy(loadName, request->moduleName);
    list_push_back(loader->loadedList, loadName);
    queue_push(loader->requests, request);
}

static void* run_Loader(Loader* loader)
{
    int* result = (int*) malloc(sizeof(int));
    *result = 0;
    // Run requests
    while(queue_size(loader->requests) > 0 && !loader->parent.isTerminated) {
        LoaderRequest* request = queue_pop(LoaderRequest*, loader->requests);
        // Run stages
        while(queue_size(request->parent.stages) > 1) {
            Stage* stage = queue_pop(Stage*, request->parent.stages);
            *result = stage->run(stage);
            free(stage);
            if(*result) {
                request->parent.free((Request*)request);
                return result;
            }
        }
        loader->decodedStack->push(loader->decodedStack, request);
    }
    // Run instanciator
    while(loader->decodedStack->size > 0 && !loader->parent.isTerminated) {
        LoaderRequest* request = NULL;
        loader->decodedStack->pop(loader->decodedStack, (void**)&request);
        Stage* stage = queue_pop(Stage*, request->parent.stages);
        *result = stage->run(stage);
        request->parent.free((Request*)request);
        free(stage);
        if(*result) {
            return result;
        }
    }
    return result;
}

static int activate_Loader(Component* component)
{
    return pthread_create(&component->thread, NULL, (void* (*) (void*))run_Loader, (void*)component);
}

static void terminate_Loader(Component* component)
{
    component->isTerminated = 1;
}

static int join_Loader(Component* component, int** resultPtr)
{
    return pthread_join(component->thread, (void**)resultPtr);
}

static void freeRequest(Request* request)
{
    request->free(request);
}

Loader* new_Loader()
{
    Loader* newLoader = (Loader*)malloc(sizeof(Loader));
    newLoader->parent.activate = activate_Loader;
    newLoader->parent.terminate = terminate_Loader;
    newLoader->parent.join = join_Loader;
    newLoader->parent.isTerminated = 0;
    newLoader->addRequest = addRequest;
    newLoader->loadedList = new_list_t(free);
    newLoader->decodedStack = new_stack((void (*)(void*))freeRequest);
    newLoader->requests = new_queue_t(freeRequest);
    return newLoader;
}

void free_Loader(Loader* loader)
{
    free_list_t(loader->loadedList);
    free_stack(loader->decodedStack);
    free_queue_t(loader->requests);
    free(loader);
}
