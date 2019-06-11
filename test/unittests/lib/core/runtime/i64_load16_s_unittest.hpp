#include <skypat/skypat.h>
#define _Bool bool
extern "C" {
#include <core/Runtime.h>
#include <dataTypes/Value.h>
#include <string.h>
}
#undef _Bool

SKYPAT_F(runtime_i64_load16_s, regular)
{
    // Prepare
    Stack stack = new_Stack();
    MemInst *memory = new_MemInst();

    memory->max = 1;

    int64_t dataArray[] = {0,     1,     -1,     4,     -4,    16,    -16,
                           64,    -64,   256,    -256,  1024,  -1024, 4096,
                           -4096, 16384, -16384, 32767, -32768
                          };
    uint32_t arraySize = sizeof(dataArray) / sizeof(dataArray[0]);
    uint32_t byteLength = sizeof(dataArray);
    uint32_t offset = 4 * sizeof(int64_t);
    uint8_t *bytePtr = (uint8_t *)dataArray;
    uint8_t zero = 0;

    // initialize memory instance
    for (uint32_t lop = 0; lop < offset; ++lop) {
        vector_push_back(memory->data, &zero);
    }
    for (uint32_t lop = 0; lop < byteLength; ++lop) {
        vector_push_back(memory->data, (bytePtr + lop));
    }

    // start testing (set memory location -> run load function -> check -> clean)
    for (uint8_t lop = 0; lop < arraySize; ++lop) {
        push_Value(stack,
                   new_i32Value(lop * sizeof(int64_t)));
        runtime_i64_load16_s(stack, memory, offset, 0);

        Value *check = NULL;

        pop_Value(stack,&check);
        EXPECT_EQ(check->value.i64, dataArray[lop]);
        free_Value(check);
    }

    // check error (65540 exceeds memory range, since max is 1)
    push_Value(stack, new_i32Value(65540));
    int ret = runtime_i64_load16_s(stack, memory, offset, 0);
    EXPECT_EQ(ret, -1);

    // clean datas
    free_MemInst(memory);
    free_Stack(stack);
}
