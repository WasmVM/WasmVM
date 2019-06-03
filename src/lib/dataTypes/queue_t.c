#include <dataTypes/queue_t_.h>

#include <stdlib.h>

void free_queue_t(queue_t thisQueue)
{
    if(thisQueue->freeElem) {
        queue_tNode cur = thisQueue->head;
        while(cur != NULL) {
            thisQueue->freeElem(cur->data);
            queue_tNode tmp = cur;
            cur = cur->next;
            free(tmp);
        }
    }
    free(thisQueue);
}

size_t queue_size(queue_t thisQueue)
{
    return thisQueue->size;
}

queue_t new_queue_t_(void (*freeElem)(void* elem))
{
    queue_t newQueue = (queue_t) malloc(sizeof(struct queue_t_));
    newQueue->head = NULL;
    newQueue->tail = NULL;
    newQueue->size = 0;
    newQueue->freeElem = freeElem;
    return newQueue;
}
void queue_push_(queue_t thisQueue, void* valuePtr)
{
    queue_tNode newNode = (queue_tNode)malloc(sizeof(struct queue_tNode_));
    newNode->data = valuePtr;
    newNode->next = NULL;
    if(thisQueue->tail != NULL) {
        thisQueue->tail->next = newNode;
    } else {
        thisQueue->head = newNode;
    }
    thisQueue->tail = newNode;
    ++thisQueue->size;
}

void* queue_pop_(queue_t thisQueue)
{
    if (thisQueue->size) {
        queue_tNode node = thisQueue->head;
        thisQueue->head = node->next;
        void* dataPtr = node->data;
        free(node);
        if(thisQueue->head == NULL) {
            thisQueue->tail = NULL;
        }
        --thisQueue->size;
        return dataPtr;
    } else {
        return NULL;
    }
}

void* queue_top_(queue_t thisQueue)
{
    if (thisQueue->size) {
        return thisQueue->head->data;
    } else {
        return NULL;
    }
}
