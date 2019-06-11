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

SKYPAT_F(runtime_f32_load, regular)
{
    // Prepare
    Stack stack = new_Stack();
    MemInst* memory = new_MemInst();
    memory->max = 1;
    float data[] = {
        0, 1, 4, 16, 64, 256, 518, 1040,
        0.125f, 1.25f, 4.375f, 16.5f, 64.625f, 256.75f, 518.8f, 1040.999f,
        -1, -4, -16, -64, -256, -518, -1040,
        -1.25f, -4.375f, -16.5f, -64.625f, -256.75f, -518.8f, -1040.999f
    };
    int32_t dataSize = sizeof(data) / sizeof(data[0]);
    uint32_t byteLen = sizeof(data);
    uint32_t offset = 4 * sizeof(float);
    uint8_t* bytePtr = (uint8_t*) data;
    uint8_t zero = 0;

    for(int32_t lop = 0; lop < offset; lop++) {
        vector_push_back(memory->data, &zero);
    }
    for(int32_t lop = 0; lop < byteLen; lop++) {
        vector_push_back(memory->data, (bytePtr + lop));
    }
    for(uint8_t lop = 0; lop < dataSize; lop++) {
        // Set load location
        push_Value(stack, new_i32Value(lop * sizeof(float)));
        // Run
        runtime_f32_load(stack, memory, offset, 0);
        // Check
        Value *check = NULL;
        pop_Value(stack, &check);
        EXPECT_EQ(check->value.f32, (float) data[lop]);
        // Clean
        free_Value(check);
    }

    // error check
    push_Value(stack, new_i32Value(65540));
    int ret = runtime_f32_load(stack, memory, offset, 0);
    EXPECT_EQ(ret, -1);

    // clean datas
    free_MemInst(memory);
    free_Stack(stack);
}
