#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <dataTypes/Value.h>
#include <core/Runtime.h>
#include <dataTypes/vector.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
}
#undef _Bool

SKYPAT_F(Runtime_i64_store_64, regular)
{
    MemInst *memory = new_MemInst();
    Stack   *stack  = new_Stack((void (*)(void*))free_Value);
    memory->max = 1;

    uint32_t memlength = 16 * sizeof(int64_t);
    uint32_t offset = 3 * sizeof(int64_t);
    uint8_t  zero   = 0;

    // init memory
    for(uint32_t lop = 0; lop < memlength + offset; lop++) {
        memory->data->push_back(memory->data, (void*) &zero);
    }

    // test store
    for(uint32_t lop = 0; lop < 5; lop++) {
        stack->entries->push(stack->entries, new_i32Value(lop * sizeof(int64_t)));
        stack->entries->push(stack->entries, new_i64Value(lop + 1));

        int ret = runtime_i64_store(stack, memory, offset, 0);
        EXPECT_EQ(ret, 0);

        int64_t *d = (int64_t *)((int8_t *)memory->data->data + lop * sizeof(int64_t) + offset);
        EXPECT_EQ(*d, lop + 1);
    }

    // error check
    Value *err      = new_i32Value(65537);
    Value *err_data = new_i64Value(1);
    stack->entries->push(stack->entries, err);
    stack->entries->push(stack->entries, err_data);
    int ret = runtime_i64_store(stack, memory, offset, 0);  // ea > 65536 check
    EXPECT_EQ(ret, -1);

    free_MemInst(memory);
    free_Stack(stack);
}
