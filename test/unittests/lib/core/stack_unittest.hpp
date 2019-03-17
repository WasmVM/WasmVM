#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <dataTypes/Value.h>
#include <core/Stack.h>
#include <dataTypes/Label.h>
#include <dataTypes/Frame.h>
#include <dataTypes/Value.h>
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

SKYPAT_F(Stack, push_Label)
{
    // Prepare
    Stack* stack = new_Stack();
    push_Label(stack, new_Label(0, 1, 2));

    // Check
    EXPECT_EQ(stack->entries->size, 1);
    EXPECT_NE(stack->entries->head, NULL);
    Label* result = NULL;
    stack->entries->top(stack->entries, (void**)&result);
    EXPECT_EQ(result->funcAddr, 0);
    EXPECT_EQ(result->instrIndex, 1);
    EXPECT_EQ(result->contInstr, 2);

    free_Stack(stack);
}