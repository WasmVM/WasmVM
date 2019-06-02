#include <core/Stack.h>
#include <dataTypes/stack.h>
#include <dataTypes/Label.h>
#include <dataTypes/Frame_.h>
#include <dataTypes/Value.h>
#include <stdlib.h>
#include <stdio.h>
#include <dataTypes/Entry.h>

static void free_entries(void* entryPtr)
{
    Entry* entry = (Entry*) entryPtr;
    switch (entry->entryType) {
        case Entry_Frame:
            free_Frame((Frame)entry);
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
    if(thisStack) {
        free_stack(thisStack->entries);
    }
    free(thisStack);
}
void push_Label(Stack* thisStack, Label label)
{
    thisStack->curLabel = label;
    thisStack->entries->push(thisStack->entries, label);
#ifndef NDEBUG
    printf("Push label of function %u\n", label_get_funcAddr(label));
#endif
}

void push_Frame(Stack* thisStack, Frame frame)
{
    thisStack->curFrame = frame;
    thisStack->curLabel = NULL;
    thisStack->entries->push(thisStack->entries, frame);
#ifndef NDEBUG
    printf("Push frame of module %s\n", frame->moduleInst->name);
#endif
}

void push_Value(Stack* thisStack, Value* value)
{
#ifndef NDEBUG
    switch (value->type) {
        case Value_i32:
            printf("Push value i32 %d\n", value->value.i32);
            break;
        case Value_i64:
            printf("Push value i64 %lld\n", value->value.i64);
            break;
        case Value_f32:
            printf("Push value f32 %f\n", value->value.f32);
            break;
        case Value_f64:
            printf("Push value f64 %lf\n", value->value.f64);
            break;
        default:
            break;
    }
#endif
    thisStack->entries->push(thisStack->entries, value);
}

int pop_Label(Stack* thisStack, Label* label)
{
    if(!thisStack->curLabel) {
        return -1;
    }
    // Save results
    stack* results = new_stack(NULL);
    vector* resultTypes = label_get_resultTypes(thisStack->curLabel);
    if(resultTypes) {
        for(uint32_t i = 0; i < resultTypes->length; ++i) {
            Value* result = NULL;
            if(pop_Value(thisStack, &result)) {
                return -3;
            }
            results->push(results, result);
        }
    }
    // Pop label
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
    // Restore results
    if(resultTypes) {
        for(uint32_t i = 0; i < resultTypes->length; ++i) {
            ValueType* type = (ValueType*)resultTypes->at(resultTypes, i);
            Value* result = NULL;
            results->pop(results, (void**)&result);
            if(result->type != *type) {
                free(result);
                return -4;
            }
            push_Value(thisStack, result);
        }
    }
    free_stack(results);
    // Update curlabel
    for(stackNode* cur = thisStack->entries->head; cur != NULL; cur = cur->next) {
        Entry* entry = (Entry*)cur->data;
        switch (entry->entryType) {
            case Entry_Label:
#ifndef NDEBUG
                printf("Pop label of function %u\n", label_get_funcAddr(*label));
#endif
                thisStack->curLabel = (Label)cur->data;
                return 0;
            case Entry_Frame:
#ifndef NDEBUG
                printf("Pop label of function %u\n", label_get_funcAddr(*label));
#endif
                thisStack->curLabel = NULL;
                return 0;
            default:
                break;
        }
    }
#ifndef NDEBUG
    printf("Pop label of function %u\n", label_get_funcAddr(*label));
#endif
    thisStack->curLabel = NULL;
    return 0;
}

int pop_Frame(Stack* thisStack, Frame* framePtr)
{
    if(!thisStack->curFrame) {
        return -1;
    }
    for(stackNode* cur = thisStack->entries->head; cur != NULL; cur = thisStack->entries->head) {
        Entry* entry = (Entry*)cur->data;
        if(entry->entryType == Entry_Frame) {
            thisStack->entries->pop(thisStack->entries, (void**)framePtr);
            break;
        } else if(entry->entryType == Entry_Value) {
            Value* value = NULL;
            pop_Value(thisStack, &value);
            free_Value(value);
        } else if(entry->entryType == Entry_Label) {
            Label label = NULL;
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
                    thisStack->curLabel = (Label)cur->data;
                }
                break;
            case Entry_Frame:
#ifndef NDEBUG
                printf("Pop frame of module %s\n", (*framePtr)->moduleInst->name);
#endif
                thisStack->curFrame = (Frame)cur->data;
                return 0;
            default:
                break;
        }
    }
    thisStack->curFrame = NULL;
#ifndef NDEBUG
    printf("Pop frame of module %s\n", (*framePtr)->moduleInst->name);
#endif
    return 0;
}

int pop_Value(Stack* thisStack, Value** value)
{
    Entry* cur = NULL;
    if(!thisStack->entries->top(thisStack->entries, (void**)&cur) && cur->entryType == Entry_Value) {
#ifndef NDEBUG
        int result = thisStack->entries->pop(thisStack->entries, (void**)value);
        switch ((*value)->type) {
            case Value_i32:
                printf("Pop value i32 %d\n", (*value)->value.i32);
                break;
            case Value_i64:
                printf("Pop value i64 %lld\n", (*value)->value.i64);
                break;
            case Value_f32:
                printf("Pop value f32 %f\n", (*value)->value.f32);
                break;
            case Value_f64:
                printf("Pop value f64 %lf\n", (*value)->value.f64);
                break;
            default:
                break;
        }
        return result;
#else
        return thisStack->entries->pop(thisStack->entries, (void**)value);
#endif
    } else {
        *value = NULL;
        return -1;
    }

}