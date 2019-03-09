#ifndef WASMVM_CORE_DEF
#define WASMVM_CORE_DEF

#include <Opcodes.h>

#include <dataTypes/Label.h>
#include <dataTypes/Frame.h>

#include <core/Stack.h>
#include <core/Store.h>

#include <instance/ModuleInst.h>
#include <instance/FuncInst.h>
#include <instance/InstrInst.h>
typedef struct Core_
{
    Store *thisStore; // share a common Store with other Core
    Stack *thisStack; // Core's private Stack
    ModuleInst *moduleInst; //Executor assign ModuleInst to each Core
    Frame *thisFrame;
    InstrInst *instrInst; // similar to program counter
    
} Core;

// create a Core obj
Core* new_Core(Store *thisStore);
void free_Core(Core *core);
// run the assigned Core obj with the assigned ModuleInst. In this way the Core can be reused, after finishing one ModuleInst
void run_Core(Core *core, ModuleInst *thisModule, FuncInst *startFunc); 

#endif