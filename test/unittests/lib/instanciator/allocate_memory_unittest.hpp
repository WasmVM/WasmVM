#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <Allocates.h>
#include <core/Store.h>
#include <structures/WasmMemory.h>
#include <instance/MemInst.h>
}
#undef _Bool

SKYPAT_F(allocate_memory, valid)
{
    // Prepare
    Store* store = new_Store();

    WasmMemory* memory = (WasmMemory*) malloc(sizeof(WasmMemory));
    memory->min = 10;
    memory->max = 20;

    // Test
    uint32_t address = allocate_Memory(memory, store);
    MemInst* memInst = (MemInst*)store->mems->at(store->mems, address);
    EXPECT_EQ(memInst->max, 20);
    EXPECT_EQ(memInst->data->length, 10 * 65536);

    // Clean
    free_Store(store);
    free(memory);
}