#include <skypat/skypat.h>

#define _Bool bool
extern "C"{
    #include <instance/FuncInst.h>
}
#undef _Bool

SKYPAT_F(FuncInst, create_delete)
{
    // Prepare
    FuncInst* funcInst = new_FuncInst();

    // Check
    EXPECT_EQ(funcInst->type.params->length, 0);
    EXPECT_EQ(funcInst->type.params->data, NULL);
    EXPECT_EQ(funcInst->type.results->length, 0);
    EXPECT_EQ(funcInst->type.results->data, NULL);
    EXPECT_EQ(funcInst->locals->length, 0);
    EXPECT_EQ(funcInst->locals->data, NULL);
    EXPECT_EQ(funcInst->code->length, 0);
    EXPECT_EQ(funcInst->code->data, NULL);
    EXPECT_EQ(funcInst->hostcode, NULL);
    EXPECT_EQ(funcInst->module, NULL);

    free_FuncInst(funcInst);
}
