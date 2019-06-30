#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <dataTypes/Value.h>
#include <core/Runtime.h>
#include <string.h>
#include <core/Store.h>
#include <dataTypes/Frame_.h>
#include <instance/ModuleInst.h>
#include <instance/GlobalInst.h>
}
#undef _Bool

SKYPAT_F(Runtime_get_global, regular)
{
    // Prepare
    Stack stack = new_Stack();
    Store store = new_Store();
    char* moduleName = (char*) malloc(sizeof(char) * 5);
    strcpy(moduleName, "Test");
    ModuleInst* module = new_ModuleInst(moduleName);
    uint32_t globalAddr = 0;
    vector_push_back(module->globaladdrs, &globalAddr);
    Frame frame = new_Frame(module);
    push_Frame(stack, frame);
    GlobalInst* global = new_GlobalInst();
    global->mut = 1;
    global->value.type = Value_i32;
    global->value.value.i32 = 27;
    vector_push_back(store->globals, global);
    free_GlobalInst(global);

    // Run
    runtime_get_global(stack, store, 0);

    // Check
    Value *check = NULL;
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.i32, 27);

    // Clean
    free_Value(check);
    free_Stack(stack);
    free_Store(store);
    free_ModuleInst(module);
}
