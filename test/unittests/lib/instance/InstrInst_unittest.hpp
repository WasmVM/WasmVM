#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <instance/InstrInst.h>
}
#undef _Bool

SKYPAT_F(InstrInst, create_delete)
{
    // Prepare
    InstrInst* instrInst = new_InstrInst();

    // Check
    EXPECT_EQ(instrInst->opcode, 0);

    free_InstrInst(instrInst);
}
