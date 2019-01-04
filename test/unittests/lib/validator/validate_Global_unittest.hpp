#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <stdint.h>
#include <stdlib.h>
#include <dataTypes/Value.h>
#include <structures/WasmGlobal.h>
#include <Validates.h>
}
#undef _Bool

SKYPAT_F(Validate_Global, valid)
{
    // Prepare
    WasmGlobal* global = (WasmGlobal*)malloc(sizeof(WasmGlobal));
    global->init.type = Value_i32;
    global->init.value.i32 = 3;
    global->init.parent.entryType = Entry_Value;
    global->mut = 1;
    global->valType = Value_i32;

    // Check
    EXPECT_EQ(validate_Global(global), 0);

    free(global);
}
