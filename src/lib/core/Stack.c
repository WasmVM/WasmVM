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
