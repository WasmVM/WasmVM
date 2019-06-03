#include "list_.h"

#include <stdlib.h>

void* list_iterator_get_(list_iterator it)
{
    return it->data;
}

list_iterator list_head(list thisList)
{
    return thisList->head;
}

list_iterator list_next(list_iterator it)
{
    return it->next;
}

size_t list_size(list thisList)
{
    return thisList->size;
}

void list_push_back_(list thislist, void* valuePtr)
{
    listNode* newNode = (listNode*)malloc(sizeof(listNode));
    newNode->data = valuePtr;
    newNode->next = NULL;
    if(thislist->tail != NULL) {
        thislist->tail->next = newNode;
    } else {
        thislist->head = newNode;
    }
    thislist->tail = newNode;
    ++thislist->size;
}

void* list_at_(list thislist, size_t index)
{
    if(index >= thislist->size) {
        return NULL;
    }
    listNode* cur = thislist->head;
    for(size_t i = 0; i < index; ++i) {
        cur = cur->next;
    }
    return cur->data;
}

int list_removeAt(list thisList, size_t index)
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

list new_list_(void (*freeElem)(void* elem))
{
    list newList = (list) malloc(sizeof(struct list_));
    newList->head = NULL;
    newList->tail = NULL;
    newList->size = 0;
    newList->freeElem = freeElem;
    return newList;
}

void free_list(list thislist)
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
