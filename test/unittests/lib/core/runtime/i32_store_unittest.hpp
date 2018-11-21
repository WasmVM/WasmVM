#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <dataTypes/Value.h>
#include <core/Runtime.h>
#include <dataTypes/vector.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
}
#undef _Bool

SKYPAT_F(Runtime_i32_store, regular)
{
    // Prepare
    vector *vector1 = new_vector(sizeof(int32_t), free);
    Stack  *stack = new_Stack();

    int32_t  i32Buffer[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
    uint32_t i32BufferLength = sizeof(i32Buffer) / sizeof(i32Buffer[0]);
    int32_t  i32OffsetBuffer[] = { 0, 0 };
    uint32_t i32Offset = sizeof(i32OffsetBuffer) / sizeof(i32OffsetBuffer[0]);

    Value *data     = NULL;
    Value *location = NULL;

    // empty vector check (no offset data)
    data = new_i32Value(30);
    location = new_i32Value(0);
    stack->entries->push(stack->entries, data);
    stack->entries->push(stack->entries, location);
    int ret = runtime_i32_store(stack, vector1, i32Offset, 0);
    EXPECT_EQ(ret, -1);

    // add offset data
    for(uint8_t lop = 0; lop < i32Offset; lop++) {
        vector1->push_back(vector1, (const void *)(i32OffsetBuffer + lop));
    }

    // empty vector check
    data = new_i32Value(30);
    location = new_i32Value(0);
    stack->entries->push(stack->entries, data);
    stack->entries->push(stack->entries, location);
    ret = runtime_i32_store(stack, vector1, i32Offset, 0);
    EXPECT_EQ(ret, -1);

    // push data to vector
    for(uint8_t lop = 0; lop < i32BufferLength; lop++) {
        vector1->push_back(vector1, (const void *)(i32Buffer + lop));
    }

    // push data and run
    for(uint8_t lop = 0; lop < i32BufferLength; lop++) {

        location = new_i32Value((int32_t)lop);
        data = new_i32Value((int32_t)(lop + 1));
        stack->entries->push(stack->entries, new_i32Value(lop));
        stack->entries->push(stack->entries, new_i32Value(lop + 1));

        runtime_i32_store(stack, vector1, i32Offset, 0);

        int32_t *retValue = (int32_t *)vector1->at(vector1, lop + i32Offset);
        EXPECT_EQ(*retValue, lop + 1);
        free(retValue);
    }

    // check store data > mems length
    location = new_i32Value(i32BufferLength + i32Offset);
    data = new_i32Value(50);
    stack->entries->push(stack->entries, data);
    stack->entries->push(stack->entries, location);
    ret = runtime_i32_store(stack, vector1, i32Offset, 0);
    EXPECT_EQ(ret, -1);

    free_vector(vector1);
    free_Stack(stack);
}