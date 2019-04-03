#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <dataTypes/list.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
}
#undef _Bool

#define wasmvm_list_foreach(node_name, list)                \
        for (struct listNode_ * node_name = (list)->head;   \
             node_name != (struct listNode_ *)NULL;         \
             node_name = node_name->next)

#define wasmvm_list_new_object(type, name, init_value)  \
        type * name = (type *)malloc(sizeof(type));     \
        *name = init_value

static int wasmvm_list_getIndex(list *pList, void *vData)
{
    int i = 0;
    wasmvm_list_foreach(node, pList) {
        if((uintptr_t)node->data == (uintptr_t)vData) {
            return i;
        }
        ++i;
    }
    return -1;
}

SKYPAT_F(list, create_delete)
{
    list *pList = new_list(free);

    EXPECT_EQ(pList->head, NULL);
    EXPECT_EQ(pList->size, 0);

    free_list(pList);
}

SKYPAT_F(list, push_back)
{
    list *pList = new_list(free);

    int ret = -1;
    wasmvm_list_new_object(int32_t, value1, 10255);
    wasmvm_list_new_object(int32_t, value2, -123);
    wasmvm_list_new_object(int32_t, value3, -9999);

    /* test push */
    pList->push_back(pList, value1);
    EXPECT_EQ(pList->size, 1);
    ret = wasmvm_list_getIndex(pList, (void *)value1);
    EXPECT_EQ(ret >= 0, true);      // check data exist in list
    ret = -1;

    pList->push_back(pList, value2);
    EXPECT_EQ(pList->size, 2);
    ret = wasmvm_list_getIndex(pList, (void *)value2);
    EXPECT_EQ(ret >= 0, true);
    ret = -1;

    pList->push_back(pList, value3);
    EXPECT_EQ(pList->size, 3);
    ret = wasmvm_list_getIndex(pList, (void *)value3);
    EXPECT_EQ(ret >= 0, true);

    /* free list */
    free_list(pList);
}

SKYPAT_F(list, at)
{
    list *pList = new_list(free);

    int index = -1;
    int32_t *at_value;

    wasmvm_list_new_object(int32_t, value1, 10255);
    wasmvm_list_new_object(int32_t, value2, -123);
    wasmvm_list_new_object(int32_t, value3, -9999);

    /* test at */
    pList->push_back(pList, value1);
    EXPECT_EQ(pList->size, 1);
    index = wasmvm_list_getIndex(pList, (void *)value1);    // get index of value1 in list
    EXPECT_EQ(index >= 0, true);
    at_value = (int32_t *)pList->at(pList, index);
    EXPECT_EQ(pList->size, 1);
    EXPECT_EQ(*at_value, *value1);

    pList->push_back(pList, value2);
    EXPECT_EQ(pList->size, 2);
    index = wasmvm_list_getIndex(pList, (void *)value2);
    EXPECT_EQ(index >= 0, true);
    at_value = (int32_t *)pList->at(pList, index);
    EXPECT_EQ(pList->size, 2);
    EXPECT_EQ(*at_value, *value2);

    pList->push_back(pList, value3);
    EXPECT_EQ(pList->size, 3);
    index = wasmvm_list_getIndex(pList, (void *)value3);
    EXPECT_EQ(index >= 0, true);
    at_value = (int32_t *)pList->at(pList, index);
    EXPECT_EQ(pList->size, 3);
    EXPECT_EQ(*at_value, *value3);

    /* free list */
    free_list(pList);
}

SKYPAT_F(list, remove_at)
{
    list *pList = new_list(free);

    int at_value;

    wasmvm_list_new_object(int32_t, value1, 10255);
    wasmvm_list_new_object(int32_t, value2, -123);
    wasmvm_list_new_object(int32_t, value3, -9999);
    wasmvm_list_new_object(int32_t, value4, 1);

    /* test size is 1 removeAt */
    pList->push_back(pList, value1);
    EXPECT_EQ(pList->size, 1);
    at_value = pList->removeAt(pList, 0);
    EXPECT_EQ(at_value, 0);
    EXPECT_EQ(pList->size, 0);

    /* test size is 3 removeAt index 1 and other */
    pList->push_back(pList, value2);
    pList->push_back(pList, value3);
    pList->push_back(pList, value4);
    EXPECT_EQ(pList->size, 3);
    at_value = pList->removeAt(pList, 1);
    EXPECT_EQ(at_value, 0);
    EXPECT_EQ(pList->size, 2);
    at_value = pList->removeAt(pList, 1);
    EXPECT_EQ(at_value, 0);
    EXPECT_EQ(pList->size, 1);
    at_value = pList->removeAt(pList, 0);
    EXPECT_EQ(at_value, 0);
    EXPECT_EQ(pList->size, 0);

    /* free list */
    free_list(pList);
}
