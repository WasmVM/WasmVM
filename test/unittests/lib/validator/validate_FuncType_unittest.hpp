#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <stdint.h>
#include <stdlib.h>
#include <dataTypes/FuncType.h>
#include <dataTypes/Value.h>
#include <Validates.h>
}
#undef _Bool

SKYPAT_F(Validate_FuncType, valid)
{
    // Prepare
    FuncType funcType = new_FuncType();
    ValueType type1 = Value_i32, type2 = Value_i64;
    vector_push_back(funcType->params, (void*)&type1);
    vector_push_back(funcType->params, (void*)&type2);

    // Check
    EXPECT_EQ(validate_FunctionType(funcType), 0);
    vector_push_back(funcType->results, (void*)&type1);
    EXPECT_EQ(validate_FunctionType(funcType), 0);

    free_FuncType(funcType);
}
