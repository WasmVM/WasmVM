#include <skypat/skypat.h>
#define _Bool bool
extern "C" {
#include <dataTypes/Value.h>
#include <core/Runtime.h>
#include <string.h>
#include <stdint.h>
}
#undef _Bool
SKYPAT_F(runtime_i32_load8_u, regular)
{
    // Prepare
    Stack* stack = new_Stack();
    int32_t str[] = { 10, 100, 150, 200, 250 };
    uint8_t strlength = sizeof(str) / sizeof(str[0]);
    int32_t snull[] = { 0, 0, 0 };
    uint32_t offset = sizeof(snull) / sizeof(snull[0]);
    vector *vector1 = new_vector(sizeof(int32_t), free);
    for(uint8_t lop = 0; lop < offset; lop++) {
        vector1->push_back(vector1, (const void *)(str + lop));
    }
    for(uint8_t lop = 0; lop < strlength; lop++) {
        vector1->push_back(vector1, (const void *)(str + lop));   // insert data to vector
    }
    for(uint8_t lop = 0; lop < strlength; lop++) {
        // Set load location
        stack->entries->push(stack->entries, new_i32Value(lop));
        // Run
        runtime_i32_load8_u(stack, vector1, offset, 0);
        // Check
        Value *check = NULL;
        stack->entries->pop(stack->entries, (void**)&check);
        EXPECT_EQ(check->value.i32, (int32_t)str[lop]);
    }
    // error check
    stack->entries->push(stack->entries, new_i32Value(strlength));
    int ret = runtime_i32_load8_u(stack, vector1, offset, 0);
    EXPECT_EQ(ret, -1);
    // clean datas
    free(vector1);
    free(stack);
}