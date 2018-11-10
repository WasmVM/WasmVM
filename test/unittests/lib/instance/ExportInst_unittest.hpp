#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <instance/ExportInst.h>
#include <dataTypes/Value.h>
}
#undef _Bool

SKYPAT_F(ExportInst, create_delete)
{
    // Prepare
    ExportInst* exportInst = new_ExportInst();

    // Check
    EXPECT_EQ(exportInst->descType, Desc_Unspecified);
    EXPECT_EQ(exportInst->name, NULL);
    EXPECT_EQ(exportInst->valueAddr, 0);

    free_ExportInst(exportInst);
}
