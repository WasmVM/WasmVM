#include <core/Stack.h>
#include <dataTypes/Label.h>
#include <dataTypes/Frame.h>
#include <dataTypes/Value.h>
#include <stdlib.h>
#include <dataTypes/Entry.h>

static void free_entries(void* entryPtr)
{
    Entry* entry = (Entry*) entryPtr;
    switch (entry->entryType) {
        case Entry_Frame:
            free_Frame((Frame*)entry);
            break;
        case Entry_Value:
            free_Value((Value*)entry);
            break;
        case Entry_Label:
        default:
            free(entry);
            break;
    }
}

Stack* new_Stack()
{
    Stack* newStack = (Stack*) malloc(sizeof(Stack));
    newStack->entries = new_stack(free_entries);
    return newStack;
}
void free_Stack(Stack* thisStack)
{
    free_stack(thisStack->entries);
    free(thisStack);
}
void push_Label(Stack* thisStack, Label* label)
{
    thisStack->curLabel = label;
    thisStack->entries->push(thisStack->entries, label);
}

void push_Frame(Stack* thisStack, Frame* frame)
{
    thisStack->curFrame = frame;
    thisStack->entries->push(thisStack->entries, frame);
}

void push_Value(Stack* thisStack, Value* value)
{
    thisStack->entries->push(thisStack->entries, value);
}

int pop_Label(Stack* thisStack, Label** label)
{
    if(!thisStack->curLabel) {
        return -1;
    }
    *label = thisStack->curLabel;
    for(stackNode* cur = thisStack->entries->head; cur != NULL; cur = cur->next) {
        if(((Entry*)cur->data)->entryType == Entry_Label) {
            thisStack->curLabel = (Label*)cur->data;
            return 0;
        }
    }
    thisStack->curLabel = NULL;
    return 0;
}
int pop_Frame(Stack* thisStack, Frame** frame)
{
    if(!thisStack->curFrame) {
        return -1;
    }
    *frame = thisStack->curFrame;
    for(stackNode* cur = thisStack->entries->head; cur != NULL; cur = cur->next) {
        if(((Entry*)cur->data)->entryType == Entry_Frame) {
            thisStack->curFrame = (Frame*)cur->data;
            return 0;
        }
    }
    thisStack->curFrame = NULL;
    return 0;
}
int pop_Value(Stack* thisStack, Value** value)
{
    Entry* cur = NULL;
    if(!thisStack->entries->pop(thisStack->entries, (void**)&cur) && cur->entryType == Entry_Value) {
        *value = (Value*)cur;
        return 0;
    } else {
        *value = NULL;
        return -1;
    }

}