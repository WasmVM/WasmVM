#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <instance/MemoryInstrInst.h>
}
#undef _Bool

SKYPAT_F(MemoryInstrInst, create_delete)
{
    // Prepare
    MemoryInstrInst* instrInst = new_MemoryInstrInst();

    // Check
    EXPECT_EQ(instrInst->offset, 0);
    EXPECT_EQ(instrInst->align, 0);
    EXPECT_EQ(instrInst->parent.opcode, 0);

    free_MemoryInstrInst(instrInst);
}
