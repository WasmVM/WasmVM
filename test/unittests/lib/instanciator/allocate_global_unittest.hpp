#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <Allocates.h>
#include <core/Store.h>
#include <dataTypes/Value.h>
#include <structures/WasmGlobal.h>
#include <instance/GlobalInst.h>
}
#undef _Bool

SKYPAT_F(allocate_global, valid)
{
    // Prepare
    Store store = new_Store();

    WasmGlobal* global = (WasmGlobal*) malloc(sizeof(WasmGlobal));
    global->mut = 1;
    global->valType = Value_i32;
    global->init.parent.entryType = Entry_Value;
    global->init.type = Value_i32;
    global->init.value.i32 = 38;

    // Test
    uint32_t address = allocate_Global(global, store);
    GlobalInst* globalInst = vector_at(GlobalInst*, store->globals, address);
    EXPECT_EQ(globalInst->mut, 1);
    EXPECT_EQ(globalInst->value.parent.entryType, Entry_Value);
    EXPECT_EQ(globalInst->value.type, Value_i32);
    EXPECT_EQ(globalInst->value.value.i32, 38);

    // Clean
    free_Store(store);
    free(global);
}