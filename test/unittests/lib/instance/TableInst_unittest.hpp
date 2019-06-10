#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <instance/TableInst.h>
}
#undef _Bool

SKYPAT_F(TableInst, create_delete)
{
    // Prepare
    TableInst* tableInst = new_TableInst();

    // Check
    EXPECT_EQ(vector_size(tableInst->elem), 0);
    EXPECT_EQ(vector_data(uint8_t*, tableInst->elem), NULL);
    EXPECT_EQ(tableInst->max, 0);

    free_TableInst(tableInst);
}
