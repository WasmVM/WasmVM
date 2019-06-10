#include <dataTypes/queue_p_.h>

#include <stdlib.h>

void free_queue_p(queue_p thisQueue)
{
    if(thisQueue == NULL) {
        return;
    }
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

size_t queue_size(queue_p thisQueue)
{
    return thisQueue->size;
}

queue_p new_queue_p_(void (*freeElem)(void* elem))
{
    queue_p newQueue = (queue_p) malloc(sizeof(struct queue_p_));
    newQueue->head = NULL;
    newQueue->tail = NULL;
    newQueue->size = 0;
    newQueue->freeElem = freeElem;
    return newQueue;
}
void queue_push_(queue_p thisQueue, void* valuePtr)
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

void* queue_pop_(queue_p thisQueue)
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

void* queue_top_(queue_p thisQueue)
{
    if (thisQueue->size) {
        return thisQueue->head->data;
    } else {
        return NULL;
    }
}
