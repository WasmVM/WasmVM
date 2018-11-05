#include <dataTypes/queue.h>

#include <stdlib.h>

static void queuePush(queue* thisqueue, void* data) {
    queueNode* newNode = (queueNode*)malloc(sizeof(queueNode));
    newNode->data = data;
    newNode->next = NULL;
    if(thisqueue->tail != NULL){
        thisqueue->tail->next = newNode;
    }else{
        thisqueue->head = newNode;
    }
    thisqueue->tail = newNode;
    ++thisqueue->size;
}

static int queuePop(queue* thisqueue, void** dataPtr) {
    if (thisqueue->size) {
        queueNode* node = thisqueue->head;
        thisqueue->head = node->next;
        *dataPtr = node->data;
        free(node);
        if(thisqueue->head == NULL){
            thisqueue->tail == NULL;
        }
        --thisqueue->size;
        return 0;
    } else {
        return -1;
    }
}

static int queueTop(queue* thisqueue, void** dataPtr) {
    if (thisqueue->size) {
        *dataPtr = thisqueue->head->data;
        return 0;
    } else {
        return -1;
    }
}

queue* new_queue(void (*freeElem)(void* elem)){
    queue* newQueue = (queue*) malloc(sizeof(queue));
    newQueue->head = NULL;
    newQueue->tail = NULL;
    newQueue->size = 0;
    newQueue->push = queuePush;
    newQueue->pop = queuePop;
    newQueue->top = queueTop;
    newQueue->freeElem = freeElem;
    return newQueue;
}

void free_queue(queue* thisqueuePtr){
    if(thisqueuePtr->freeElem){
        queueNode* cur = thisqueuePtr->head;
        while(cur != NULL){
            thisqueuePtr->freeElem(cur->data);
        }
    }
    free(thisqueuePtr);
}