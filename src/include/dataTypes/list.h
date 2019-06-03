#ifndef WASMVM_DATATYPES_LIST_DEF
#define WASMVM_DATATYPES_LIST_DEF

#include <stddef.h>

typedef struct list_ * list;
typedef struct listNode_ * list_iterator;

void free_list(list thisList);
int list_removeAt(list thisList, size_t index);
list_iterator list_head(list thisList);
list_iterator list_next(list_iterator it);
size_t list_size(list thisList);

list new_list_(void (*freeElem)(void* elem));
void list_push_back_(list thisList, void* valuePtr);
void* list_at_(list thisList, size_t index);
void* list_iterator_get_(list_iterator it);

#define new_list(freeElemFunc) new_list_((void (*)(void*))freeElemFunc)
#define list_push_back(thisList, valuePtr) list_push_back_(thisList, (void*) valuePtr)
#define list_at(T, thisList, index) (T)list_at_(thisList, index)
#define list_iterator_get(T, it) (T)list_iterator_get_(it)

#endif