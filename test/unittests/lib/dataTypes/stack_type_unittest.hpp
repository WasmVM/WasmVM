#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <dataTypes/stack_p.h>
#include <stdlib.h>
}
#undef _Bool

SKYPAT_F(stack, create_delete)
{
    // Prepare
    stack* newStack = new_stack(free);

    // Check
    EXPECT_EQ(newStack->head, NULL);
    EXPECT_EQ(newStack->size, 0);

    free_stack(newStack);
}

SKYPAT_F(stack, push)
{
    // Prepare
    stack* newStack = new_stack(free);

    int *data1 = (int*)malloc(sizeof(int));
    *data1 = 1;
    int *data2 = (int*)malloc(sizeof(int));
    *data2 = 2;

    // Check
    newStack->push(newStack, data1);
    EXPECT_EQ(newStack->size, 1);
    EXPECT_EQ(newStack->head->data, data1);
    EXPECT_EQ(((int*)newStack->head->data)[0], 1);
    newStack->push(newStack, data2);
    EXPECT_EQ(newStack->size, 2);
    EXPECT_EQ(newStack->head->data, data2);
    EXPECT_EQ(newStack->head->next->data, data1);
    EXPECT_EQ(((int*)newStack->head->data)[0], 2);

    free_stack(newStack);
}

SKYPAT_F(stack, pop)
{
    // Prepare
    stack* newStack = new_stack(free);

    int *data1 = (int*)malloc(sizeof(int));
    *data1 = 1;
    int *data2 = (int*)malloc(sizeof(int));
    *data2 = 2;

    // Check
    newStack->push(newStack, data1);
    newStack->push(newStack, data2);
    int *check1 = NULL, *check2=NULL;
    newStack->pop(newStack, (void**)&check2);
    EXPECT_EQ(newStack->size, 1);
    EXPECT_EQ(*check2, 2);
    newStack->pop(newStack, (void**)&check1);
    EXPECT_EQ(newStack->size, 0);
    EXPECT_EQ(*check1, 1);

    free(check1);
    free(check2);
    free_stack(newStack);
}

SKYPAT_F(stack, top)
{
    // Prepare
    stack* newStack = new_stack(free);

    int *data1 = (int*)malloc(sizeof(int));
    *data1 = 1;
    int *data2 = (int*)malloc(sizeof(int));
    *data2 = 2;

    // Check
    newStack->push(newStack, data1);
    newStack->push(newStack, data2);
    int *check1 = NULL, *check2=NULL;
    newStack->top(newStack, (void**)&check2);
    EXPECT_EQ(*check2, 2);
    newStack->pop(newStack, (void**)&check2);
    newStack->top(newStack, (void**)&check1);
    EXPECT_EQ(*check1, 1);

    free(check2);
    free_stack(newStack);
}
