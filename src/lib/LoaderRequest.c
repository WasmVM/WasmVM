#include <LoaderRequest.h>

#include <stdlib.h>
#include <string.h>
#include <dataTypes/queue.h>
#include <Decoder.h>
#include <Validator.h>
#include <Instanciator.h>

LoaderRequest* new_LoaderRequest(const char* moduleName, Component* loader, Executor* executor)
{
    LoaderRequest* request = (LoaderRequest*) malloc(sizeof(LoaderRequest));
    request->parent.stages = new_queue(free);
    request->parent.free = (void(*)(Request*))free_LoaderRequest;
    request->moduleName = (char*) malloc(sizeof(char)*strlen(moduleName));
    strcpy(request->moduleName, moduleName);
    WasmModule* module = new_WasmModule(request->moduleName);
    // Decoder
    Decoder* decoder = new_Decoder(loader, executor);
    decoder->parent.input = NULL;
    decoder->parent.output = module;
    request->parent.stages->push(request->parent.stages, (void*)decoder);
    // Validator
    Validator* validator = new_Validator(module);
    request->parent.stages->push(request->parent.stages, (void*)validator);
    // Instanciator
    Instanciator* instanciator = new_Instanciator(module, executor);
    request->parent.stages->push(request->parent.stages, (void*)instanciator);
    return request;
}

void free_LoaderRequest(LoaderRequest* request)
{
    free_queue(request->parent.stages);
    free(request);
}