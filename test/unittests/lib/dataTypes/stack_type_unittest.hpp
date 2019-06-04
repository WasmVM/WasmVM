#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <dataTypes/stack_p_.h>
#include <stdlib.h>
}
#undef _Bool

SKYPAT_F(stack, create_delete)
{
    // Prepare
    stack_p newStack = new_stack_p(free);

    // Check
    EXPECT_EQ(newStack->head, NULL);
    EXPECT_EQ(newStack->size, 0);

    free_stack_p(newStack);
}

SKYPAT_F(stack, push)
{
    // Prepare
    stack_p newStack = new_stack_p(free);

    int *data1 = (int*)malloc(sizeof(int));
    *data1 = 1;
    int *data2 = (int*)malloc(sizeof(int));
    *data2 = 2;

    // Check
    stack_push(newStack, data1);
    EXPECT_EQ(newStack->size, 1);
    EXPECT_EQ(newStack->head->data, data1);
    EXPECT_EQ(((int*)newStack->head->data)[0], 1);
    stack_push(newStack, data2);
    EXPECT_EQ(newStack->size, 2);
    EXPECT_EQ(newStack->head->data, data2);
    EXPECT_EQ(newStack->head->next->data, data1);
    EXPECT_EQ(((int*)newStack->head->data)[0], 2);

    free_stack_p(newStack);
}

SKYPAT_F(stack, pop)
{
    // Prepare
    stack_p newStack = new_stack_p(free);

    int *data1 = (int*)malloc(sizeof(int));
    *data1 = 1;
    int *data2 = (int*)malloc(sizeof(int));
    *data2 = 2;

    // Check
    stack_push(newStack, data1);
    stack_push(newStack, data2);
    int *check2 = stack_pop(int*, newStack);
    EXPECT_EQ(newStack->size, 1);
    EXPECT_EQ(*check2, 2);
    int *check1 = stack_pop(int*, newStack);
    EXPECT_EQ(newStack->size, 0);
    EXPECT_EQ(*check1, 1);

    free(check1);
    free(check2);
    free_stack_p(newStack);
}

SKYPAT_F(stack, top)
{
    // Prepare
    stack_p newStack = new_stack_p(free);

    int *data1 = (int*)malloc(sizeof(int));
    *data1 = 1;
    int *data2 = (int*)malloc(sizeof(int));
    *data2 = 2;

    // Check
    stack_push(newStack, data1);
    stack_push(newStack, data2);
    int* check2 = stack_top(int*, newStack);
    EXPECT_EQ(*check2, 2);
    check2 = stack_pop(int*, newStack);
    int* check1 = stack_top(int*, newStack);
    EXPECT_EQ(*check1, 1);

    free(check2);
    free_stack_p(newStack);
}
