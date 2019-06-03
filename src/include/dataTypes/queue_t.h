#ifndef WASMVM_DATATYPES_QUEUE_DEF
#define WASMVM_DATATYPES_QUEUE_DEF

#include <stddef.h>

typedef struct queue_t_ * queue_t;

void free_queue_t(queue_t thisQueue);
size_t queue_size(queue_t thisQueue);

queue_t new_queue_t_(void (*freeElem)(void* elem));
void queue_push_(queue_t thisQueue, void* valuePtr);
void* queue_pop_(queue_t thisQueue);
void* queue_top_(queue_t thisQueue);

#define new_queue_t(freeElemFunc) new_queue_t_((void(*)(void*))freeElemFunc)
#define queue_push(thisQueue, valuePtr) queue_push_(thisQueue, (void*) valuePtr)
#define queue_pop(T, thisQueue) (T)queue_pop_(thisQueue)
#define queue_top(T, thisQueue) (T)queue_top_(thisQueue)

#endif