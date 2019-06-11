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

SKYPAT_F(Runtime_f64_store, regular)
{
    MemInst *memory = new_MemInst();
    Stack stack  = new_Stack();
    memory->max = 1;
    double data[] = {
        0, 1, 4, 16, 64, 256, 518, 1040,
        0.125, 1.25, 4.3757293, 16.5, 64.625, 256.75, 518.857121, 1040.999, 1333.3333, 20135.23147
        -1, -4, -16, -64, -256, -518, -1040,
        -0.125, -1.25, -4.3757293, -16.5, -64.625, -256.75, -518.857121, -1040.999, -1333.3333, -20135.23147
    };
    uint32_t dataSize = sizeof(data) / sizeof(data[0]);
    uint32_t memlength = sizeof(data);
    uint32_t offset = 3 * sizeof(double);
    uint8_t  zero   = 0;

    // init memory
    for(uint32_t lop = 0; lop < memlength + offset; lop++) {
        vector_push_back(memory->data, &zero);
    }

    // test store
    for(uint32_t lop = 0; lop < dataSize; lop++) {
        push_Value(stack, new_i32Value(lop * sizeof(double)));
        push_Value(stack, new_f64Value(data[lop]));

        int ret = runtime_f64_store(stack, memory, offset, 0);
        EXPECT_EQ(ret, 0);

        double *d = (double *) ( vector_data(int8_t *, memory->data) + lop * sizeof(double) + offset);
        EXPECT_EQ(*d, data[lop]);
    }

    // error check
    Value *err      = new_i32Value(65537);
    Value *err_data = new_f64Value(1);
    push_Value(stack, err);
    push_Value(stack, err_data);
    int ret = runtime_f64_store(stack, memory, offset, 0);  // ea > 65536 check
    EXPECT_EQ(ret, -1);

    free_MemInst(memory);
    free_Stack(stack);
}
