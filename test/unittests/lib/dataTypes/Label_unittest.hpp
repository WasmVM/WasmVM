#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <dataTypes/Label.h>
}
#undef _Bool

SKYPAT_F(Label, create_delete)
{
    // Prepare
    Label *label = new_Label(0, 1, 2);

    // Check
    EXPECT_EQ(label->parent.entryType, Entry_Label);
    EXPECT_EQ(label->funcAddr, 0);
    EXPECT_EQ(label->instrIndex, 1);
    EXPECT_EQ(label->contInstr, 2);

    free_Label(label);
}