#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <dataTypes/list_p_.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
}
#undef _Bool

#define wasmvm_list_foreach(node_name, list_p)                \
        for (list_iterator node_name = list_head(list_p);     \
             list_iterator_get(void*, node_name) != NULL;     \
             node_name = list_iterator_next(node_name))

#define wasmvm_list_new_object(type, name, init_value)  \
        type * name = (type *)malloc(sizeof(type));     \
        *name = init_value

static int wasmvm_list_getIndex(list_p thisList, void *vData)
{
    int i = 0;
    wasmvm_list_foreach(node, thisList) {
        if(list_iterator_get(uintptr_t, node) == (uintptr_t)vData) {
            return i;
        }
        ++i;
    }
    return -1;
}

SKYPAT_F(list_p, create_delete)
{
    list_p pList = new_list_p(free);

    EXPECT_EQ(pList->head, NULL);
    EXPECT_EQ(pList->size, 0);

    free_list_p(pList);
}

SKYPAT_F(list_p, push_back)
{
    list_p pList = new_list_p(free);

    int ret = -1;
    wasmvm_list_new_object(int32_t, value1, 10255);
    wasmvm_list_new_object(int32_t, value2, -123);
    wasmvm_list_new_object(int32_t, value3, -9999);

    /* test push */
    list_push_back(pList, value1);
    EXPECT_EQ(pList->size, 1);
    ret = wasmvm_list_getIndex(pList, (void *)value1);
    EXPECT_EQ(ret >= 0, true);      // check data exist in list_p
    ret = -1;

    list_push_back(pList, value2);
    EXPECT_EQ(pList->size, 2);
    ret = wasmvm_list_getIndex(pList, (void *)value2);
    EXPECT_EQ(ret >= 0, true);
    ret = -1;

    list_push_back(pList, value3);
    EXPECT_EQ(pList->size, 3);
    ret = wasmvm_list_getIndex(pList, (void *)value3);
    EXPECT_EQ(ret >= 0, true);

    /* free list_p */
    free_list_p(pList);
}

SKYPAT_F(list_p, at)
{
    list_p pList = new_list_p(free);

    int index = -1;
    int32_t *at_value;

    wasmvm_list_new_object(int32_t, value1, 10255);
    wasmvm_list_new_object(int32_t, value2, -123);
    wasmvm_list_new_object(int32_t, value3, -9999);

    /* test at */
    list_push_back(pList, value1);
    EXPECT_EQ(pList->size, 1);
    index = wasmvm_list_getIndex(pList, (void *)value1);    // get index of value1 in list_p
    EXPECT_EQ(index >= 0, true);
    at_value = list_at(int32_t*, pList, index);
    EXPECT_EQ(pList->size, 1);
    EXPECT_EQ(*at_value, *value1);

    list_push_back(pList, value2);
    EXPECT_EQ(pList->size, 2);
    index = wasmvm_list_getIndex(pList, (void *)value2);
    EXPECT_EQ(index >= 0, true);
    at_value = list_at(int32_t*, pList, index);
    EXPECT_EQ(pList->size, 2);
    EXPECT_EQ(*at_value, *value2);

    list_push_back(pList, value3);
    EXPECT_EQ(pList->size, 3);
    index = wasmvm_list_getIndex(pList, (void *)value3);
    EXPECT_EQ(index >= 0, true);
    at_value = list_at(int32_t*, pList, index);
    EXPECT_EQ(pList->size, 3);
    EXPECT_EQ(*at_value, *value3);

    /* free list_p */
    free_list_p(pList);
}

SKYPAT_F(list_p, remove_at)
{
    list_p pList = new_list_p(free);

    int at_value;

    wasmvm_list_new_object(int32_t, value1, 10255);
    wasmvm_list_new_object(int32_t, value2, -123);
    wasmvm_list_new_object(int32_t, value3, -9999);
    wasmvm_list_new_object(int32_t, value4, 1);

    /* test size is 1 removeAt */
    list_push_back(pList, value1);
    EXPECT_EQ(pList->size, 1);
    at_value = list_removeAt(pList, 0);
    EXPECT_EQ(at_value, 0);
    EXPECT_EQ(pList->size, 0);

    /* test size is 3 removeAt index 1 and other */
    list_push_back(pList, value2);
    list_push_back(pList, value3);
    list_push_back(pList, value4);
    EXPECT_EQ(pList->size, 3);
    at_value = list_removeAt(pList, 1);
    EXPECT_EQ(at_value, 0);
    EXPECT_EQ(pList->size, 2);
    at_value = list_removeAt(pList, 1);
    EXPECT_EQ(at_value, 0);
    EXPECT_EQ(pList->size, 1);
    at_value = list_removeAt(pList, 0);
    EXPECT_EQ(at_value, 0);
    EXPECT_EQ(pList->size, 0);

    /* free list_p */
    free_list_p(pList);
}
