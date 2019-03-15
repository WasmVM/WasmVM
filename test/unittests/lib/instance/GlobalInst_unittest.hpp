#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <instance/GlobalInst.h>
}
#undef _Bool

SKYPAT_F(GlobalInst, create_delete)
{
    // Prepare
    GlobalInst* globalInst = new_GlobalInst();

    // Check
    EXPECT_EQ(globalInst->mut, 0);
    EXPECT_EQ(globalInst->value.type, Value_Unspecified);
    EXPECT_EQ(globalInst->value.parent.entryType, Entry_Value);

    free_GlobalInst(globalInst);
}
