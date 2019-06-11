#include <skypat/skypat.h>
#define _Bool bool
extern "C" {
#include <dataTypes/Value.h>
#include <core/Runtime.h>
#include <instance/MemInst.h>
#include <string.h>
#include <stdint.h>
}
#undef _Bool

SKYPAT_F(runtime_f64_load, regular)
{
    // Prepare
    Stack stack = new_Stack();
    MemInst* memory = new_MemInst();
    memory->max = 1;
    double data[] = {
        0, 1, 4, 16, 64, 256, 518, 1040,
        0.125, 1.25, 4.3757293, 16.5, 64.625, 256.75, 518.857121, 1040.999, 1333.3333, 20135.23147
        -1, -4, -16, -64, -256, -518, -1040,
        -0.125, -1.25, -4.3757293, -16.5, -64.625, -256.75, -518.857121, -1040.999, -1333.3333, -20135.23147
    };
    int32_t dataSize = sizeof(data) / sizeof(data[0]);
    uint32_t byteLen = sizeof(data);
    uint32_t offset = 4 * sizeof(double);
    uint8_t* bytePtr = (uint8_t*) data;
    uint8_t zero = 0;

    for(int32_t lop = 0; lop < offset; lop++) {
        vector_push_back(memory->data, (const void *) &zero);
    }
    for(int32_t lop = 0; lop < byteLen; lop++) {
        vector_push_back(memory->data, (const void *) (bytePtr + lop));
    }
    for(uint8_t lop = 0; lop < dataSize; lop++) {
        // Set load location
        push_Value(stack, new_i32Value(lop * sizeof(double)));
        // Run
        runtime_f64_load(stack, memory, offset, 0);
        // Check
        Value *check = NULL;
        pop_Value(stack, &check);
        EXPECT_EQ(check->value.f64, (double) data[lop]);
        // Clean
        free_Value(check);
    }

    // error check
    push_Value(stack, new_i32Value(65540));
    int ret = runtime_f64_load(stack, memory, offset, 0);
    EXPECT_EQ(ret, -1);

    // clean datas
    free_MemInst(memory);
    free_Stack(stack);
}
