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

SKYPAT_F(Runtime_f32_store, regular)
{
    MemInst *memory = new_MemInst();
    Stack stack  = new_Stack();
    memory->max = 1;
    float data[] = {
        0, 1, 4, 16, 64, 256, 518, 1040,
        0.125f, 1.25f, 4.375f, 16.5f, 64.625f, 256.75f, 518.8f, 1040.999f,
        -1, -4, -16, -64, -256, -518, -1040,
        -1.25f, -4.375f, -16.5f, -64.625f, -256.75f, -518.8f, -1040.999f
    };
    uint32_t dataSize = sizeof(data) / sizeof(data[0]);
    uint32_t memlength = sizeof(data);
    uint32_t offset = 3 * sizeof(float);
    uint8_t  zero   = 0;

    // init memory
    for(uint32_t lop = 0; lop < memlength + offset; lop++) {
        vector_push_back(memory->data, (void*) &zero);
    }

    // test store
    for(uint32_t lop = 0; lop < dataSize; lop++) {
        push_Value(stack, new_i32Value(lop * sizeof(float)));
        push_Value(stack, new_f32Value(data[lop]));

        int ret = runtime_f32_store(stack, memory, offset, 0);
        EXPECT_EQ(ret, 0);

        float *d = (float *) (vector_data(int8_t *, memory->data) + lop * sizeof(float) + offset);
        EXPECT_EQ(*d, data[lop]);
    }

    // error check
    Value *err      = new_i32Value(65537);
    Value *err_data = new_f32Value(1);
    push_Value(stack, err);
    push_Value(stack, err_data);
    int ret = runtime_f32_store(stack, memory, offset, 0);  // ea > 65536 check
    EXPECT_EQ(ret, -1);

    free_MemInst(memory);
    free_Stack(stack);
}
