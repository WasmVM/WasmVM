#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <stdlib.h>
#include <string.h>
#include <instance/FuncInst.h>
#include <instance/ModuleInst.h>
#include <dataTypes/FuncType.h>
}
#undef _Bool

SKYPAT_F(FuncInst, create_delete)
{
    // Prepare
    char* moduleName = (char*) malloc(sizeof(char) * 5);
    strcpy(moduleName, "test");
    ModuleInst* module = new_ModuleInst(moduleName);
    FuncType type = new_FuncType();
    FuncInst* funcInst = new_FuncInst(module, type);

    // Check
    EXPECT_EQ(funcInst->type, type);
    EXPECT_EQ(funcInst->locals->length, 0);
    EXPECT_EQ(funcInst->locals->data, NULL);
    EXPECT_EQ(funcInst->code->size, 0);
    EXPECT_EQ(funcInst->code->head, NULL);
    EXPECT_EQ(funcInst->hostcode, NULL);
    EXPECT_EQ(funcInst->module, module);

    free_FuncInst(funcInst);
}
