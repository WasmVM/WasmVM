#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <dataTypes/FuncType.h>
}
#undef _Bool

SKYPAT_F(FuncType, create_delete)
{
    // Prepare
    FuncType funcType = new_FuncType();

    // Check
    EXPECT_EQ(vector_size(funcType->params), 0);
    EXPECT_EQ(vector_data(uint8_t*,funcType->params), NULL);
    EXPECT_EQ(vector_size(funcType->results), 0);
    EXPECT_EQ(vector_data(uint8_t*,funcType->results), NULL);

    free_FuncType(funcType);
}