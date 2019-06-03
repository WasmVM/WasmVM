#include "list_t_.h"

#include <stdlib.h>

void* list_iterator_get_(list_iterator it)
{
    return it->data;
}

list_iterator list_head(list_t thisList)
{
    return thisList->head;
}

list_iterator list_next(list_iterator it)
{
    return it->next;
}

size_t list_size(list_t thisList)
{
    return thisList->size;
}

void list_push_back_(list_t thislist_t, void* valuePtr)
{
    listNode* newNode = (listNode*)malloc(sizeof(listNode));
    newNode->data = valuePtr;
    newNode->next = NULL;
    if(thislist_t->tail != NULL) {
        thislist_t->tail->next = newNode;
    } else {
        thislist_t->head = newNode;
    }
    thislist_t->tail = newNode;
    ++thislist_t->size;
}

void* list_at_(list_t thislist_t, size_t index)
{
    if(index >= thislist_t->size) {
        return NULL;
    }
    listNode* cur = thislist_t->head;
    for(size_t i = 0; i < index; ++i) {
        cur = cur->next;
    }
    return cur->data;
}

int list_removeAt(list_t thisList, size_t index)
{
    if(index >= thisList->size) {
        return -1;
    }
    listNode** previous = &thisList->head;
    listNode* cur = thisList->head;
    for(size_t i = 0; i < index; ++i) {
        previous = &cur->next;
        cur = cur->next;
    }
    *previous = cur->next;
    if(*previous == (listNode *)NULL) {
        thisList->tail = (listNode *)NULL;
    }
    if(thisList->freeElem) {
        thisList->freeElem(cur->data);
    }
    free(cur);
    --thisList->size;
    return 0;
}

list_t new_list_(void (*freeElem)(void* elem))
{
    list_t newList = (list_t) malloc(sizeof(struct list_));
    newList->head = NULL;
    newList->tail = NULL;
    newList->size = 0;
    newList->freeElem = freeElem;
    return newList;
}

void free_list(list_t thislist)
{
    if(thislist->freeElem) {
        listNode* cur = thislist->head;
        while(cur != NULL) {
            thislist->freeElem(cur->data);
            listNode* tmp = cur;
            cur = cur->next;
            free(tmp);
        }
    }
    free(thislist);
}
