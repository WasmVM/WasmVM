#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <instance/ControlInstrInst.h>
#include <dataTypes/Value.h>
}
#undef _Bool

SKYPAT_F(ControlInstrInst, create_delete)
{
    // Prepare
    ControlInstrInst* instrInst = new_ControlInstrInst();

    // Check
    EXPECT_EQ(vector_size(instrInst->resultTypes), 0);
    EXPECT_EQ(vector_data(uint8_t*, instrInst->resultTypes), NULL);
    EXPECT_EQ(vector_size(instrInst->indices), 0);
    EXPECT_EQ(vector_data(uint8_t*, instrInst->indices), NULL);

    free_ControlInstrInst(instrInst);
}
