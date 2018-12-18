#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <stdint.h>
#include <stdlib.h>
#include <dataTypes/Value.h>
#include <structures/WasmTable.h>
#include <Validates.h>
}
#undef _Bool

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