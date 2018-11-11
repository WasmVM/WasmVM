#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <dataTypes/FuncType.h>
}
#undef _Bool

SKYPAT_F(FuncType, create_delete)
{
    // Prepare
    FuncType* funcType = new_FuncType();

    // Check
    EXPECT_EQ(funcType->params->length, 0);
    EXPECT_EQ(funcType->params->data, NULL);
    EXPECT_EQ(funcType->results->length, 0);
    EXPECT_EQ(funcType->results->data, NULL);

    free_FuncType(funcType);
}