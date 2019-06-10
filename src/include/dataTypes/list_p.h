#ifndef WASMVM_DATATYPES_LIST_DEF
#define WASMVM_DATATYPES_LIST_DEF

#include <stddef.h>

typedef struct list_p_ * list_p;
typedef struct listNode_ * list_iterator;

void free_list_p(list_p thisList);
int list_removeAt(list_p thisList, size_t index);
list_iterator list_head(list_p thisList);
list_iterator list_iterator_next(list_iterator it);
size_t list_size(list_p thisList);

list_p new_list_p_(void (*freeElem)(void* elem));
void list_push_back_(list_p thisList, void* valuePtr);
void* list_at_(list_p thisList, size_t index);
void* list_iterator_get_(list_iterator it);

#define new_list_p(freeElemFunc) new_list_p_((void (*)(void*))freeElemFunc)
#define list_push_back(thisList, valuePtr) list_push_back_(thisList, (void*) valuePtr)
#define list_at(T, thisList, index) (T)list_at_(thisList, index)
#define list_iterator_get(T, it) (T)list_iterator_get_(it)

#endif