#ifndef WASMVM_DATATYPES_QUEUE_DEF
#define WASMVM_DATATYPES_QUEUE_DEF

typedef struct queueNode_ {
    void* data;
    struct queueNode_* next;
} queueNode;

typedef struct queue_{
    queueNode* head;
    queueNode* tail;
    unsigned int size;
    void (*freeElem)(void* elem);
    void (*push)(struct queue_* thisQueue, void* data);
    // If success return 0, or return -1
    int (*pop)(struct queue_* thisQueue, void** dataPtr);
    int (*top)(struct queue_* thisQueue, void** dataPtr);
} queue;

queue* new_queue(void (*freeElem)(void* elem));
void free_queue(queue* thisqueuePtr);


#endif