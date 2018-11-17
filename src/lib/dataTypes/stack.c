#include <dataTypes/stack.h>

#include <stdlib.h>

static void stackPush(stack* thisstack, void* data)
{
    stackNode* newNode = (stackNode*)malloc(sizeof(stackNode));
    newNode->data = data;
    newNode->next = thisstack->head;
    thisstack->head = newNode;
    ++thisstack->size;
}

static int stackPop(stack* thisstack, void** dataPtr)
{
    if (thisstack->size) {
        stackNode* node = thisstack->head;
        thisstack->head = node->next;
        *dataPtr = node->data;
        free(node);
        --thisstack->size;
        return 0;
    } else {
        return -1;
    }
}

static int stackTop(stack* thisstack, void** dataPtr)
{
    if (thisstack->size) {
        *dataPtr = thisstack->head->data;
        return 0;
    } else {
        return -1;
    }
}

stack* new_stack()
{
    stack* newstack = (stack*)malloc(sizeof(stack));
    newstack->head = NULL;
    newstack->size = 0;
    newstack->pop = stackPop;
    newstack->push = stackPush;
    newstack->top = stackTop;
    return newstack;
}
void free_stack(stack* thisstack)
{
    free(thisstack);
}