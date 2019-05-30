#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <string.h>
#include <stdlib.h>
#include <dataTypes/Frame_.h>
#include <instance/ModuleInst.h>
}
#undef _Bool

SKYPAT_F(Frame, create_delete)
{
    // Prepare
    char* moduleName = (char*)malloc(sizeof(char) * 5);
    strcpy(moduleName, "Test");
    ModuleInst* module = new_ModuleInst(moduleName);
    Frame frame = new_Frame(module);

    // Check
    EXPECT_EQ(frame->locals->length, 0);
    EXPECT_EQ(frame->locals->data, NULL);
    EXPECT_EQ(frame->moduleInst, module);

    free_Frame(frame);
}