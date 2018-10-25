#include <skypat/skypat.h>

#define _Bool bool
extern "C"{
    #include <instance/ControlInstrInst.h>
    #include <dataTypes/Value.h>
}
#undef _Bool

SKYPAT_F(ControlInstrInst, create_delete)
{
    // Prepare
    ControlInstrInst* instrInst = new_ControlInstrInst();

    // Check
    EXPECT_EQ(instrInst->resultType, Value_Unspecified);
    EXPECT_EQ(instrInst->indices->length, 0);
    EXPECT_EQ(instrInst->indices->data, NULL);

    free_ControlInstrInst(instrInst);
}
