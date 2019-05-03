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
    newStack->curLabel = NULL;
    newStack->curFrame = NULL;
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
    thisStack->curLabel = NULL;
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
    for(stackNode* cur = thisStack->entries->head; cur != NULL; cur = thisStack->entries->head) {
        Entry* entry = (Entry*)cur->data;
        if(entry->entryType == Entry_Label) {
            thisStack->entries->pop(thisStack->entries, (void**)label);
            break;
        } else if(entry->entryType == Entry_Value) {
            Value* value = NULL;
            pop_Value(thisStack, &value);
            free_Value(value);
        } else {
            thisStack->curLabel = NULL;
            return -2;
        }
    }
    for(stackNode* cur = thisStack->entries->head; cur != NULL; cur = cur->next) {
        Entry* entry = (Entry*)cur->data;
        switch (entry->entryType) {
            case Entry_Label:
                thisStack->curLabel = (Label*)cur->data;
                return 0;
            case Entry_Frame:
                thisStack->curLabel = NULL;
                return 0;
            default:
                break;
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
    for(stackNode* cur = thisStack->entries->head; cur != NULL; cur = thisStack->entries->head) {
        Entry* entry = (Entry*)cur->data;
        if(entry->entryType == Entry_Frame) {
            thisStack->entries->pop(thisStack->entries, (void**)frame);
            break;
        } else if(entry->entryType == Entry_Value) {
            Value* value = NULL;
            pop_Value(thisStack, &value);
            free_Value(value);
        } else if(entry->entryType == Entry_Label) {
            Label* label = NULL;
            pop_Label(thisStack, &label);
            free_Label(label);
        } else {
            thisStack->curFrame = NULL;
            return -2;
        }
    }
    thisStack->curLabel = NULL;
    for(stackNode* cur = thisStack->entries->head; cur != NULL; cur = cur->next) {
        Entry* entry = (Entry*)cur->data;
        switch (entry->entryType) {
            case Entry_Label:
                if(!thisStack->curLabel) {
                    thisStack->curLabel = (Label*)cur->data;
                }
                break;
            case Entry_Frame:
                thisStack->curFrame = (Frame*)cur->data;
                return 0;
            default:
                break;
        }
    }
    thisStack->curFrame = NULL;
    return 0;
}

int pop_Value(Stack* thisStack, Value** value)
{
    Entry* cur = NULL;
    if(!thisStack->entries->top(thisStack->entries, (void**)&cur) && cur->entryType == Entry_Value) {
        return thisStack->entries->pop(thisStack->entries, (void**)value);
    } else {
        *value = NULL;
        return -1;
    }

}