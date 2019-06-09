#include <core/Stack.h>
#include <dataTypes/stack_p.h>
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
    newStack->entries = new_stack_p(free_entries);
    newStack->curLabel = NULL;
    newStack->curFrame = NULL;
    return newStack;
}
void free_Stack(Stack* thisStack)
{
    if(thisStack) {
        free_stack_p(thisStack->entries);
    }
    free(thisStack);
}
void push_Label(Stack* thisStack, Label label)
{
    thisStack->curLabel = label;
    stack_push(thisStack->entries, label);
#ifndef NDEBUG
    printf("Push label of function %u\n", label_get_funcAddr(label));
#endif
}

void push_Frame(Stack* thisStack, Frame frame)
{
    thisStack->curFrame = frame;
    thisStack->curLabel = NULL;
    stack_push(thisStack->entries, frame);
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
    stack_push(thisStack->entries, value);
}

int pop_Label(Stack* thisStack, Label* label)
{
    if(!thisStack->curLabel) {
        return -1;
    }
    // Save results
    stack_p results = new_stack_p(NULL);
    vector_p resultTypes = label_get_resultTypes(thisStack->curLabel);
    if(resultTypes) {
        for(uint32_t i = 0; i < vector_size(resultTypes); ++i) {
            Value* result = NULL;
            if(pop_Value(thisStack, &result)) {
                return -3;
            }
            stack_push(results, result);
        }
    }
    // Pop label
    for(Entry* entry = stack_top(Entry*, thisStack->entries); entry != NULL; entry = stack_top(Entry*, thisStack->entries)) {
        if(entry->entryType == Entry_Label) {
            *label = stack_pop(Label, thisStack->entries);
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
        for(uint32_t i = 0; i < vector_size(resultTypes); ++i) {
            ValueType* type = vector_at(ValueType*, resultTypes, i);
            Value* result = stack_pop(Value*, results);
            if(result->type != *type) {
                free(result);
                return -4;
            }
            push_Value(thisStack, result);
        }
    }
    free_stack_p(results);
    // Update curlabel
    for(stack_iterator it = stack_head(thisStack->entries); it != NULL; it = stack_iterator_next(it)) {
        Entry* entry = stack_iterator_get(Entry*, it);
        switch (entry->entryType) {
            case Entry_Label:
#ifndef NDEBUG
                printf("Pop label of function %u\n", label_get_funcAddr(*label));
#endif
                thisStack->curLabel = (Label)entry;
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
    for(stack_iterator it = stack_head(thisStack->entries); it != NULL; it = stack_head(thisStack->entries)) {
        Entry* entry = stack_iterator_get(Entry*,it);
        if(entry->entryType == Entry_Frame) {
            *framePtr = stack_pop(Frame, thisStack->entries);
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
    for(stack_iterator it = stack_head(thisStack->entries); it != NULL; it = stack_iterator_next(it)) {
        Entry* entry = stack_iterator_get(Entry*,it);
        switch (entry->entryType) {
            case Entry_Label:
                if(!thisStack->curLabel) {
                    thisStack->curLabel = (Label)entry;
                }
                break;
            case Entry_Frame:
#ifndef NDEBUG
                printf("Pop frame of module %s\n", (*framePtr)->moduleInst->name);
#endif
                thisStack->curFrame = (Frame)entry;
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
    Entry* cur = stack_top(Entry*, thisStack->entries);
    if(cur && (cur->entryType == Entry_Value)) {
        *value = stack_pop(Value*, thisStack->entries);
#ifndef NDEBUG
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
#endif
        return -(*value == NULL);
    } else {
        *value = NULL;
        return -2;
    }

}