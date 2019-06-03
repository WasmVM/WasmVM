#ifndef WASMVM_DATATYPES_QUEUE_DEF
#define WASMVM_DATATYPES_QUEUE_DEF

#include <stddef.h>

typedef struct queue_ * queue;

void free_queue(queue thisQueue);
size_t queue_size(queue thisQueue);

queue new_queue_(void (*freeElem)(void* elem));
void queue_push_(queue thisQueue, void* valuePtr);
void* queue_pop_(queue thisQueue);
void* queue_top_(queue thisQueue);

#define new_queue(freeElemFunc) new_queue_((void(*)(void*))freeElemFunc)
#define queue_push(thisQueue, valuePtr) queue_push_(thisQueue, (void*) valuePtr)
#define queue_pop(T, thisQueue) (T)queue_pop_(thisQueue);
#define queue_top(T, thisQueue) (T)queue_top_(thisQueue);

#endif