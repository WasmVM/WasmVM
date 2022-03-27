#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <instances/ParametricInstrInst.h>
}
#undef _Bool

SKYPAT_F(ParametricInstrInst, create_delete)
{
    // Prepare
    ParametricInstrInst* instrInst = new_ParametricInstrInst();

    // Check
    EXPECT_EQ(instrInst->opcode, 0);

    free_ParametricInstrInst(instrInst);
}
