#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <dataTypes/Frame.h>
}
#undef _Bool

SKYPAT_F(Frame, create_delete)
{
    // Prepare
    Frame *frame = new_Frame();

    // Check
    EXPECT_EQ(frame->localVals->length, 0);
    EXPECT_EQ(frame->localVals->data, NULL);
    EXPECT_EQ(frame->moduleInst->funcaddrs->length, 0);
    EXPECT_EQ(frame->moduleInst->funcaddrs->data, NULL);
    EXPECT_EQ(frame->moduleInst->tableaddrs->length, 0);
    EXPECT_EQ(frame->moduleInst->tableaddrs->data, NULL);
    EXPECT_EQ(frame->moduleInst->memaddrs->length, 0);
    EXPECT_EQ(frame->moduleInst->memaddrs->data, NULL);
    EXPECT_EQ(frame->moduleInst->globaladdrs->length, 0);
    EXPECT_EQ(frame->moduleInst->globaladdrs->data, NULL);

    free_Frame(frame);
}