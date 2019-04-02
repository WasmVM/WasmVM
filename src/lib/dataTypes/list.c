#include <dataTypes/list.h>

#include <stdlib.h>

static void listPushBack(list* thislist, void* data)
{
    listNode* newNode = (listNode*)malloc(sizeof(listNode));
    newNode->data = data;
    newNode->next = NULL;
    if(thislist->tail != NULL) {
        thislist->tail->next = newNode;
    } else {
        thislist->head = newNode;
    }
    thislist->tail = newNode;
    ++thislist->size;
}

static void* listAt(list* thislist, size_t index)
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

static int listRemoveAt(list* thislist, size_t index)
{
    if(index >= thislist->size) {
        return -1;
    }
    listNode** previous = &thislist->head;
    listNode* cur = thislist->head;
    for(size_t i = 0; i < index; ++i) {
        previous = &cur->next;
        cur = cur->next;
    }
    *previous = cur->next;
    if(*previous == (listNode *)NULL) {
        thislist->tail = (listNode *)NULL;
    }
    if(thislist->freeElem) {
        thislist->freeElem(cur->data);
    }
    free(cur);
    --thislist->size;
    return 0;
}

list* new_list(void (*freeElem)(void* elem))
{
    list* newQueue = (list*) malloc(sizeof(list));
    newQueue->head = NULL;
    newQueue->tail = NULL;
    newQueue->size = 0;
    newQueue->push_back = (void (*)(list*, const void*))listPushBack;
    newQueue->at = listAt;
    newQueue->removeAt = listRemoveAt;
    newQueue->freeElem = freeElem;
    return newQueue;
}

void free_list(list* thislistPtr)
{
    if(thislistPtr->freeElem) {
        listNode* cur = thislistPtr->head;
        listNode* tmp;
        while(cur != NULL) {
            thislistPtr->freeElem(cur->data);
            tmp = cur;
            cur = cur->next;
            free(tmp);
        }
    }
    free(thislistPtr);
}
