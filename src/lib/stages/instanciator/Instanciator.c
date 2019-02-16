#include <Instanciator.h>

#include <stdlib.h>

static void setInput(Instanciator* instanciator, void* input)
{
    instanciator->parent.input = input;
}
static void* getOutput(Instanciator* instanciator)
{
    return instanciator->parent.output;
}
static int runInstanciator(Instanciator* instanciator)
{
    return 0; // TODO:
}

static void freeInstanciator(Instanciator* instanciator)
{
    free_Store(instanciator->store);
    free_ModuleInst(instanciator->moduleInst);
    free(instanciator);
}

Instanciator* new_Instanciator(WasmModule* module, Store* store, ModuleInst* moduleInst)
{
    Instanciator* instanciator = (Instanciator*) malloc(sizeof(Instanciator));
    instanciator->parent.input = module;
    instanciator->parent.output = NULL;
    instanciator->parent.setInput = setInput;
    instanciator->parent.getOutput = getOutput;
    instanciator->parent.run = runInstanciator;
    instanciator->parent.free = (void(*)(void*))freeInstanciator;
    instanciator->moduleInst = moduleInst;
    instanciator->store = store;
    return instanciator;
}