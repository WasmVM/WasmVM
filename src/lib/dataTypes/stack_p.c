#include <dataTypes/stack_p_.h>

#include <stdlib.h>

void free_stack_p(stack_p thisstack)
{
    if(thisstack == NULL) {
        return;
    }
    if (thisstack->freeElem) {
        while (thisstack->head != NULL) {
            stackNode node = thisstack->head;
            thisstack->head = node->next;
            thisstack->freeElem(node->data);
            free(node);
        }
    }
    free(thisstack);
}

size_t stack_size(stack_p thisstack)
{
    return thisstack->size;
}

stack_p new_stack_p_(void (*freeElem)(void* elem))
{
    stack_p newstack = (stack_p)malloc(sizeof(struct stack_p_));
    newstack->head = NULL;
    newstack->tail = NULL;
    newstack->size = 0;
    newstack->freeElem = freeElem;
    return newstack;
}

void stack_push_(stack_p thisstack, void* valuePtr)
{
    stackNode newNode = (stackNode)malloc(sizeof(struct stackNode_));
    newNode->data = valuePtr;
    newNode->next = thisstack->head;
    thisstack->head = newNode;
    ++thisstack->size;
}

void* stack_pop_(stack_p thisstack)
{
    if (thisstack->size) {
        stackNode node = thisstack->head;
        thisstack->head = node->next;
        void* dataPtr = node->data;
        free(node);
        --thisstack->size;
        return dataPtr;
    } else {
        return NULL;
    }
}

void* stack_top_(stack_p thisstack)
{
    if (thisstack->size) {
        return thisstack->head->data;
    } else {
        return NULL;
    }
}

stack_iterator stack_head(stack_p thisstack)
{
    return thisstack->head;
}

stack_iterator stack_iterator_next(stack_iterator it)
{
    return it->next;
}

void* stack_iterator_get_(stack_iterator it)
{
    return it->data;
}