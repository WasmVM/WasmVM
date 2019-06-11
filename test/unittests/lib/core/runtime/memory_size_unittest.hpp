#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <dataTypes/Value.h>
#include <core/Runtime.h>
#include <stdint.h>
#include <stddef.h>
}
#undef _Bool

SKYPAT_F(memory_size, regular)
{
    MemInst *memory = new_MemInst();
    Stack stack = new_Stack();
    memory->max = 3;

    uint64_t pageSize = 64 * 1024;

    // realloc 1 page to memory instance
    vector_resize(memory->data, pageSize);

    // testing
    runtime_memory_size(stack, memory);

    // checking
    Value *check = NULL;
    pop_Value(stack, &check);
    EXPECT_EQ(check->value.i32, 1);

    // realloc 1 page to memory instance
    vector_resize(memory->data, pageSize * 2);

    // testing
    runtime_memory_size(stack, memory);

    // checking
    Value *check2 = NULL;
    pop_Value(stack, &check2);
    EXPECT_EQ(check2->value.i32, 2);

    free_Value(check);
    free_Value(check2);
    free_MemInst(memory);
    free_Stack(stack);
}
