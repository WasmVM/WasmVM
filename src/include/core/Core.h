#ifndef WASMVM_CORE_DEF
#define WASMVM_CORE_DEF

#include "Stack.h"
#include "Store.h"
#include "ModuleInst.h"
#include "Opcodes.h"
#include "Label.h"

typedef struct Core_
{
    Store *thisStore; // share a common Store with other Core
    Stack *thisStack; // share a common Stack with other Core
    ModuleInst *moduleInst; //Executor assign ModuleInst to each Core
} Core;

// create a Core obj
Core* new_Core(Store *thisStore, stack *thisStack);
void free_Core(Core *core);
// run the assigned Core obj with the assigned ModuleInst. In this way the Core can be reused, after finishing one ModuleInst
void run_Core(Core *core, ModuleInst *thisModule);

#endif