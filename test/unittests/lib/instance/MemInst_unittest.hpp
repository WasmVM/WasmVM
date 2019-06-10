#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <instance/MemInst.h>
}
#undef _Bool

SKYPAT_F(MemInst, create_delete)
{
    // Prepare
    MemInst* memInst = new_MemInst();

    // Check
    EXPECT_EQ(vector_size(memInst->data), 0);
    EXPECT_EQ(vector_data(uint8_t*, memInst->data), NULL);
    EXPECT_EQ(memInst->max, 0);

    free_MemInst(memInst);
}
