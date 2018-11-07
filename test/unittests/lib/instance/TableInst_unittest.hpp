#include <skypat/skypat.h>

#define _Bool bool
extern "C"{
    #include <instance/TableInst.h>
}
#undef _Bool

SKYPAT_F(TableInst, create_delete)
{
    // Prepare
    TableInst* tableInst = new_TableInst();

    // Check
    EXPECT_EQ(tableInst->elem->length, 0);
    EXPECT_EQ(tableInst->elem->data, NULL);
    EXPECT_EQ(tableInst->max, 0);

    free_TableInst(tableInst);
}
