#ifndef WASMVM_DATATYPES_QUEUE_DEF
#define WASMVM_DATATYPES_QUEUE_DEF

#include <stddef.h>

typedef struct queue_p_ * queue_p;

void free_queue_p(queue_p thisQueue);
size_t queue_size(queue_p thisQueue);

queue_p new_queue_p_(void (*freeElem)(void* elem));
void queue_push_(queue_p thisQueue, void* valuePtr);
void* queue_pop_(queue_p thisQueue);
void* queue_top_(queue_p thisQueue);

#define new_queue_p(freeElemFunc) new_queue_p_((void(*)(void*))freeElemFunc)
#define queue_push(thisQueue, valuePtr) queue_push_(thisQueue, (void*) valuePtr)
#define queue_pop(T, thisQueue) (T)queue_pop_(thisQueue)
#define queue_top(T, thisQueue) (T)queue_top_(thisQueue)

#endif