#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <stdint.h>
#include <stdlib.h>
#include <dataTypes/FuncType.h>
#include <dataTypes/Value.h>
#include <structures/WasmTable.h>
#include <Validates.h>
}
#undef _Bool

SKYPAT_F(Validate_FuncType, valid)
{
    // Prepare
    FuncType* funcType = new_FuncType();
    ValueType type1 = Value_i32, type2 = Value_i64;
    funcType->params->push_back(funcType->params, (void*)&type1);
    funcType->params->push_back(funcType->params, (void*)&type2);

    // Check
    EXPECT_EQ(validate_FunctionType(funcType), 0);
    funcType->results->push_back(funcType->results, (void*)&type1);
    EXPECT_EQ(validate_FunctionType(funcType), 0);

    free_FuncType(funcType);
}

SKYPAT_F(Validate_FuncType, too_much_result)
{
    // Prepare
    FuncType* funcType = new_FuncType();
    ValueType type1 = Value_i32, type2 = Value_i64;
    funcType->params->push_back(funcType->params, (void*)&type1);
    funcType->params->push_back(funcType->params, (void*)&type2);

    // Check
    funcType->results->push_back(funcType->results, (void*)&type1);
    funcType->results->push_back(funcType->results, (void*)&type2);
    EXPECT_NE(validate_FunctionType(funcType), 0);

    free_FuncType(funcType);
}

SKYPAT_F(Validate_Table, valid)
{
    // Prepare
    WasmTable* table = (WasmTable*)malloc(sizeof(WasmTable));
    table->min = 2;
    table->max = 3;

    // Check
    EXPECT_EQ(validate_Table(table), 0);
    table->max = 0;
    EXPECT_EQ(validate_Table(table), 0);

    free(table);
}

SKYPAT_F(Validate_Table, max_smaller_than_min)
{
    // Prepare
    WasmTable* table = (WasmTable*)malloc(sizeof(WasmTable));
    table->min = 4;
    table->max = 3;

    // Check
    EXPECT_NE(validate_Table(table), 0);

    free(table);
}