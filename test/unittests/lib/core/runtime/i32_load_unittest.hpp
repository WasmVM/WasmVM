#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <dataTypes/Value.h>
#include <core/Runtime.h>
#include <instance/MemInst.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
}
#undef _Bool

SKYPAT_F(runtime_i32_load_32, regular)
{
    // Prepare
    Stack stack = new_Stack();
    MemInst* memory = new_MemInst();
    memory->max = 1;
    int32_t data[] = { 1000, 2000, 3000, 4000, 5000 };
    uint32_t byteLen = 5 * sizeof(int32_t);
    uint32_t offset = 4 * sizeof(int32_t);
    uint8_t* bytePtr = (uint8_t*) data;
    uint8_t zero = 0;

    // insert offset data to vector
    for(uint32_t lop = 0; lop < offset; lop++) {
        vector_push_back(memory->data, &zero);
    }

    // insert data to vector
    for(uint32_t lop = 0; lop < byteLen; lop++) {
        vector_push_back(memory->data, (bytePtr + lop));
    }

    for(uint32_t lop = 0; lop < 5; lop++) {
        // Set load location
        push_Value(stack, new_i32Value(lop * sizeof(int32_t)));

        // Run
        int ret = runtime_i32_load(stack, memory, offset, 0);
        EXPECT_EQ(ret, 0);

        // Check
        Value *check = NULL;
        pop_Value(stack,&check);
        EXPECT_EQ(check->value.i32, data[lop]);

        // Clean
        free_Value(check);
    }

    // error check
    push_Value(stack, new_i32Value(65537));
    int ret = runtime_i32_load(stack, memory, offset, 0);
    EXPECT_EQ(ret, -1);

    // clean datas
    free_MemInst(memory);
    free_Stack(stack);
}
