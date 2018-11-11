#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <instance/NumericInstrInst.h>
#include <dataTypes/Value.h>
}
#undef _Bool

SKYPAT_F(NumericInstrInst, create_delete)
{
    // Prepare
    NumericInstrInst* instrInst = new_NumericInstrInst();

    // Check
    EXPECT_EQ(instrInst->constant.parent.entryType, Entry_Value);
    EXPECT_EQ(instrInst->constant.type, Value_Unspecified);
    EXPECT_EQ(instrInst->parent.opcode, 0);

    free_NumericInstrInst(instrInst);
}
