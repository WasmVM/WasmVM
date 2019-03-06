#include <core/Core.h>
#include <stdint.h>
#include <stdlib.h>

Core* new_Core(Store *thisStore, stack *thisStack)
{
    Core *core = (Core *) malloc(sizeof(Core));
    core->thisStore = thisStore;
    core->thisStack = thisStack;

    return core;
}

void free_Core(Core* thisCore)
{
    // free the current Core only, since Store and Stack may be used by other core.
    free(thisCore);
}

void run_Core(Core *core, ModuleInst *thisModule)
{
    core->moduleInst = thisModule;
}