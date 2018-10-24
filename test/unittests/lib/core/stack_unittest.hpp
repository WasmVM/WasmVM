#include <skypat/skypat.h>

#define _Bool bool
extern "C"{
    #include <dataTypes/Value.h>
    #include <core/Stack.h>
}
#undef _Bool

SKYPAT_F(Stack, create_delete)
{
    // Prepare
    Stack* stack = new_Stack();

    // Check
    EXPECT_EQ(stack->entries->size, 0);
    EXPECT_EQ(stack->entries->head, NULL);

    free_Stack(stack);
}
