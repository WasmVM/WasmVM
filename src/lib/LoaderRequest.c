#include <LoaderRequest.h>

#include <stdlib.h>
#include <string.h>
#include <dataTypes/queue_p.h>
#include <Decoder.h>
#include <Validator.h>
#include <Instanciator.h>

LoaderRequest new_LoaderRequest_(const char* moduleName, Component* loader, Executor executor)
{
    LoaderRequest request = (LoaderRequest) malloc(sizeof(struct LoaderRequest_));
    request->parent.stages = new_queue_p(free);
    request->parent.free = (void(*)(Request*))free_LoaderRequest;
    request->moduleName = (char*) malloc(sizeof(char)*strlen(moduleName));
    strcpy(request->moduleName, moduleName);
    WasmModule* module = new_WasmModule(request->moduleName);
    // Decoder
    Decoder decoder = new_Decoder(loader, executor, module);
    queue_push(request->parent.stages, decoder);
    // Validator
    Validator* validator = new_Validator(module);
    queue_push(request->parent.stages, validator);
    // Instanciator
    Instanciator instanciator = new_Instanciator(module, executor);
    queue_push(request->parent.stages, instanciator);
    return request;
}

void free_LoaderRequest(LoaderRequest request)
{
    free_queue_p(request->parent.stages);
    free(request);
}