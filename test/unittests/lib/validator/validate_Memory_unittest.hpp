#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <stdint.h>
#include <stdlib.h>
#include <dataTypes/Value.h>
#include <structures/WasmMemory.h>
#include <Validates.h>
}
#undef _Bool

SKYPAT_F(Validate_Memory, valid)
{
    // Prepare
    WasmMemory* memory = (WasmMemory*)malloc(sizeof(WasmMemory));
    memory->min = 2;
    memory->max = 3;

    // Check
    EXPECT_EQ(validate_Memory(memory), 0);
    memory->max = 0;
    EXPECT_EQ(validate_Memory(memory), 0);

    free(memory);
}

SKYPAT_F(Validate_Memory, max_smaller_than_min)
{
    // Prepare
    WasmMemory* memory = (WasmMemory*)malloc(sizeof(WasmMemory));
    memory->min = 4;
    memory->max = 3;

    // Check
    EXPECT_NE(validate_Memory(memory), 0);

    free(memory);
}

SKYPAT_F(Validate_Memory, min_bigger_than_limit)
{
    // Prepare
    WasmMemory* memory = (WasmMemory*)malloc(sizeof(WasmMemory));
    memory->min = UINT16_MAX + 4;
    memory->max = 0;

    // Check
    EXPECT_NE(validate_Memory(memory), 0);

    free(memory);
}

SKYPAT_F(Validate_Memory, max_bigger_than_limit)
{
    // Prepare
    WasmMemory* memory = (WasmMemory*)malloc(sizeof(WasmMemory));
    memory->min = 1;
    memory->max = UINT16_MAX + 4;

    // Check
    EXPECT_NE(validate_Memory(memory), 0);

    free(memory);
}