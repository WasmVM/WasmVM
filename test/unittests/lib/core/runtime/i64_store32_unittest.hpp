#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <dataTypes/Value.h>
#include <core/Runtime.h>
#include <dataTypes/vector_p.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
}
#undef _Bool

SKYPAT_F(Runtime_i64_store32, regular)
{
    MemInst *memory = new_MemInst();
    Stack stack  = new_Stack();
    memory->max = 1;

    uint32_t memlength = 16 * sizeof(int64_t);
    uint32_t offset = 3 * sizeof(int64_t);
    uint8_t  zero   = 0;

    // init memory
    for(uint32_t lop = 0; lop < memlength + offset; lop++) {
        vector_push_back(memory->data, &zero);
    }

    // test store
    for(uint32_t lop = 0; lop < 5; lop++) {
        push_Value(stack, new_i32Value(lop * sizeof(int64_t)));
        push_Value(stack, new_i64Value(lop + 1));

        int ret = runtime_i64_store32(stack, memory, offset, 0);
        EXPECT_EQ(ret, 0);

        int32_t *d = (int32_t *)(vector_data(int8_t*, memory->data) + lop * sizeof(int64_t) + offset);
        EXPECT_EQ(*d, (int32_t)(lop + 1));
    }

    // error check
    Value *err      = new_i32Value(65537);
    Value *err_data = new_i64Value(1);
    push_Value(stack, err);
    push_Value(stack, err_data);
    int ret = runtime_i64_store32(stack, memory, offset, 0);  // ea > 65536 check
    EXPECT_EQ(ret, -1);

    free_MemInst(memory);
    free_Stack(stack);
}
