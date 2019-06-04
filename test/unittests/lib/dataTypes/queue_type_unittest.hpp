#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <dataTypes/queue_p_.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
}
#undef _Bool

SKYPAT_F(queue_p, create_delete)
{
    queue_p pQueue = new_queue_p(free);

    EXPECT_EQ(pQueue->head, NULL);
    EXPECT_EQ(pQueue->size, 0);

    free_queue_p(pQueue);
}

SKYPAT_F(queue_p, push)
{
    queue_p pQueue = new_queue_p(free);

    int32_t *value1 = (int32_t *)malloc(sizeof(uint32_t));
    *value1 = 1023;
    int32_t *value2 = (int32_t *)malloc(sizeof(uint32_t));
    *value2 = -123;
    int32_t *value3 = (int32_t *)malloc(sizeof(uint32_t));
    *value3 = -100;

    queue_push(pQueue, value1);
    EXPECT_EQ(pQueue->size, 1);
    EXPECT_EQ(pQueue->head->data, value1);
    EXPECT_EQ(*(int32_t *)pQueue->head->data, 1023);

    queue_push(pQueue, value2);
    EXPECT_EQ(pQueue->size, 2);
    EXPECT_EQ(pQueue->head->data, value1);
    EXPECT_EQ(pQueue->head->next->data, value2);
    EXPECT_EQ(*(int32_t *)pQueue->head->next->data, -123);

    queue_push(pQueue, value3);
    EXPECT_EQ(pQueue->size, 3);
    EXPECT_EQ(pQueue->head->data, value1);
    EXPECT_EQ(pQueue->head->next->data, value2);
    EXPECT_EQ(pQueue->head->next->next->data, value3);
    EXPECT_EQ(*(int32_t *)pQueue->head->next->next->data, -100);

    /* free object */
    free_queue_p(pQueue);
}

SKYPAT_F(queue_p, pop)
{
    int test = 0;
    queue_p pQueue = new_queue_p(free);

    /* push data */
    int32_t *pop_value1 = (int32_t *)malloc(sizeof(int32_t));
    *pop_value1 = 1023;
    int32_t *pop_value2 = (int32_t *)malloc(sizeof(int32_t));
    *pop_value2 = -123;
    int32_t *pop_value3 = (int32_t *)malloc(sizeof(int32_t));
    *pop_value3 = -100;

    queue_push(pQueue, pop_value1);
    queue_push(pQueue, pop_value2);
    queue_push(pQueue, pop_value3);

    /* test pop data */
    int32_t *pop_ans = NULL;

    EXPECT_EQ(pQueue->head->data, pop_value1);
    EXPECT_EQ(pQueue->tail->data, pop_value3);
    EXPECT_EQ(pQueue->size, 3);

    pop_ans = queue_pop(int32_t*, pQueue);
    EXPECT_EQ(*pop_ans, *pop_value1);
    EXPECT_EQ(pQueue->head->data, pop_value2);
    EXPECT_EQ(pQueue->tail->data, pop_value3);
    EXPECT_EQ(pQueue->size, 2);
    free(pop_ans);
    pop_ans = NULL;

    pop_ans = queue_pop(int32_t*, pQueue);
    EXPECT_EQ(*pop_ans, *pop_value2);
    EXPECT_EQ(pQueue->head->data, pop_value3);
    EXPECT_EQ(pQueue->tail->data, pop_value3);
    EXPECT_EQ(pQueue->size, 1);
    free(pop_ans);
    pop_ans = NULL;

    pop_ans = queue_pop(int32_t*, pQueue);
    EXPECT_EQ(*pop_ans, *pop_value3);
    EXPECT_EQ(pQueue->size, 0);
    free(pop_ans);
    pop_ans = NULL;

    /* error check */
    pop_ans = queue_pop(int32_t*, pQueue);
    EXPECT_EQ(pop_ans, NULL);

    /* free object */
    free_queue_p(pQueue);
}

SKYPAT_F(queue_p, top)
{
    int test = 0;
    queue_p pQueue = new_queue_p(free);

    /* push data */
    int32_t *top_value1 = (int32_t *)malloc(sizeof(int32_t));
    *top_value1 = 1023;
    int32_t *top_value2 = (int32_t *)malloc(sizeof(int32_t));
    *top_value2 = -123;
    int32_t *top_value3 = (int32_t *)malloc(sizeof(int32_t));
    *top_value3 = -100;

    queue_push(pQueue, top_value1);
    queue_push(pQueue, top_value2);
    queue_push(pQueue, top_value3);

    /* test top method */
    int32_t *top_ans = NULL;

    EXPECT_EQ(pQueue->size, 3);                     // check size
    top_ans = queue_top(int32_t*, pQueue);
    EXPECT_EQ(*top_ans, *top_value1);               // test top value
    EXPECT_EQ(pQueue->size, 3);                     // check size

    top_ans = queue_pop(int32_t*, pQueue);
    free(top_ans);
    top_ans = NULL;

    EXPECT_EQ(pQueue->size, 2);
    top_ans = queue_top(int32_t*, pQueue);
    EXPECT_EQ(*top_ans, *top_value2);
    EXPECT_EQ(pQueue->size, 2);

    top_ans = queue_pop(int32_t*, pQueue);
    free(top_ans);
    top_ans = NULL;

    EXPECT_EQ(pQueue->size, 1);
    top_ans = queue_top(int32_t*, pQueue);
    EXPECT_EQ(*top_ans, *top_value3);
    EXPECT_EQ(pQueue->size, 1);

    top_ans = queue_pop(int32_t*, pQueue);
    free(top_ans);
    top_ans = NULL;

    /* error check */
    top_ans = queue_top(int32_t*, pQueue);
    EXPECT_EQ(top_ans, NULL);

    /* free value */
    free_queue_p(pQueue);
}
