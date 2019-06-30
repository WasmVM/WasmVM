#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <dataTypes/Value.h>
#include <core/Runtime.h>
#include <string.h>
#include <dataTypes/Frame_.h>
#include <instance/ModuleInst.h>
}
#undef _Bool

SKYPAT_F(Runtime_get_local, regular)
{
    // Prepare
    Stack stack = new_Stack();
    char* moduleName = (char*) malloc(sizeof(char) * 5);
    strcpy(moduleName, "Test");
    ModuleInst* module = new_ModuleInst(moduleName);
    Frame frame = new_Frame(module);
    Value* local = new_i32Value(18);
    vector_push_back(frame->locals, local);
    free(local);
    push_Frame(stack, frame);

    // Run
    runtime_get_local(stack, 0);

    // Check
    Value *check = NULL;
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.i32, 18);

    // Clean
    free_Value(check);
    free_Stack(stack);
    free_ModuleInst(module);
}
