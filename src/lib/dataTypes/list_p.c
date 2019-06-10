#include "list_p_.h"

#include <stdlib.h>

void* list_iterator_get_(list_iterator it)
{
    return it->data;
}

list_iterator list_head(list_p thisList)
{
    return thisList->head;
}

list_iterator list_iterator_next(list_iterator it)
{
    return it->next;
}

size_t list_size(list_p thisList)
{
    return thisList->size;
}

void list_push_back_(list_p thislist_p, void* valuePtr)
{
    listNode* newNode = (listNode*)malloc(sizeof(listNode));
    newNode->data = valuePtr;
    newNode->next = NULL;
    if(thislist_p->tail != NULL) {
        thislist_p->tail->next = newNode;
    } else {
        thislist_p->head = newNode;
    }
    thislist_p->tail = newNode;
    ++thislist_p->size;
}

void* list_at_(list_p thislist_p, size_t index)
{
    if(index >= thislist_p->size) {
        return NULL;
    }
    listNode* cur = thislist_p->head;
    for(size_t i = 0; i < index; ++i) {
        cur = cur->next;
    }
    return cur->data;
}

int list_removeAt(list_p thisList, size_t index)
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

list_p new_list_p_(void (*freeElem)(void* elem))
{
    list_p newList = (list_p) malloc(sizeof(struct list_p_));
    newList->head = NULL;
    newList->tail = NULL;
    newList->size = 0;
    newList->freeElem = freeElem;
    return newList;
}

void free_list_p(list_p thislist)
{
    if(thislist == NULL) {
        return;
    }
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
