#include <dataTypes/queue_.h>

#include <stdlib.h>

void free_queue(queue thisQueue)
{
    if(thisQueue->freeElem) {
        queueNode cur = thisQueue->head;
        while(cur != NULL) {
            thisQueue->freeElem(cur->data);
            queueNode tmp = cur;
            cur = cur->next;
            free(tmp);
        }
    }
    free(thisQueue);
}

size_t queue_size(queue thisQueue)
{
    return thisQueue->size;
}

queue new_queue_(void (*freeElem)(void* elem))
{
    queue newQueue = (queue) malloc(sizeof(struct queue_));
    newQueue->head = NULL;
    newQueue->tail = NULL;
    newQueue->size = 0;
    newQueue->freeElem = freeElem;
    return newQueue;
}
void queue_push_(queue thisQueue, void* valuePtr)
{
    queueNode newNode = (queueNode)malloc(sizeof(struct queueNode_));
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

void* queue_pop_(queue thisQueue)
{
    if (thisQueue->size) {
        queueNode node = thisQueue->head;
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

void* queue_top_(queue thisQueue)
{
    if (thisQueue->size) {
        return thisQueue->head->data;
    } else {
        return NULL;
    }
}
