#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <core/Runtime.h>
#include <dataTypes/Value.h>
#include <stdint.h>
#include <stddef.h>
}
#undef _Bool

SKYPAT_F(memory_grow, regular)
{
    Stack stack = new_Stack();
    MemInst *memory = new_MemInst();

    uint32_t pageSize = 64 * 1024;

    memory->max = 4;
    uint32_t currect_sz = 0;

    // testing grow memory to 1 page
    Value *check = NULL;
    push_Value(stack, new_i32Value(1));
    runtime_memory_grow(stack, memory);
    pop_Value(stack, &check);
    EXPECT_EQ(check->value.i32, currect_sz);
    currect_sz = 1;
    free_Value(check);

    // testing grow memory to 2 page
    push_Value(stack, new_i32Value(1));
    runtime_memory_grow(stack, memory);
    pop_Value(stack, &check);
    EXPECT_EQ(check->value.i32, currect_sz);
    currect_sz = 2;
    free_Value(check);

    // testing grow memory to 3 page
    push_Value(stack, new_i32Value(1));
    runtime_memory_grow(stack, memory);
    pop_Value(stack, &check);
    EXPECT_EQ(check->value.i32, currect_sz);
    currect_sz = 3;
    free_Value(check);

    // error testing grow <= memory:sz
    push_Value(stack, new_i32Value(0));
    runtime_memory_grow(stack, memory);
    pop_Value(stack, &check);
    EXPECT_EQ(check->value.i32, -1);
    free_Value(check);

    // error testing grow >= memory:max
    push_Value(stack, new_i32Value(2));
    runtime_memory_grow(stack, memory);
    pop_Value(stack, &check);
    EXPECT_EQ(check->value.i32, -1);
    free_Value(check);

    // error testing grow by error type
    push_Value(stack, new_i64Value(1));
    runtime_memory_grow(stack, memory);
    pop_Value(stack, &check);

    // free data
    free_Stack(stack);
    free_MemInst(memory);
}
