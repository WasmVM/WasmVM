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

SKYPAT_F(Runtime_set_global, regular)
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
    push_Value(stack, new_i32Value(81));
    runtime_set_global(stack, store, 0);

    // Check
    global = vector_at(GlobalInst*, store->globals, 0);
    EXPECT_EQ(global->value.value.i32, 81);

    // Clean
    free_Stack(stack);
    free_Store(store);
    free_ModuleInst(module);
}
