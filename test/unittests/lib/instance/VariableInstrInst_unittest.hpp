#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <instance/VariableInstrInst.h>
#include <dataTypes/Value.h>
}
#undef _Bool

SKYPAT_F(VariableInstrInst, create_delete)
{
    // Prepare
    VariableInstrInst* instrInst = new_VariableInstrInst();

    // Check
    EXPECT_EQ(instrInst->index, 0);
    EXPECT_EQ(instrInst->parent.opcode, 0);

    free_VariableInstrInst(instrInst);
}
