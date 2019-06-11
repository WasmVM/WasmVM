#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <Allocates.h>
#include <core/Store.h>
#include <structures/WasmTable.h>
#include <instance/TableInst.h>
}
#undef _Bool

SKYPAT_F(allocate_table, valid)
{
    // Prepare
    Store store = new_Store();

    WasmTable* table = (WasmTable*) malloc(sizeof(WasmTable));
    table->elemType = 0;
    table->min = 10;
    table->max = 20;

    // Test
    uint32_t address = allocate_Table(table, store);
    TableInst* tableInst = vector_at(TableInst*, store->tables, address);
    EXPECT_EQ(tableInst->max, 20);
    EXPECT_EQ(vector_size(tableInst->elem), 10);

    // Clean
    free_Store(store);
    free(table);
}