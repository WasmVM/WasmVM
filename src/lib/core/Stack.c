#include <core/Stack_.h>
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

Stack new_Stack()
{
    Stack newStack = (Stack) malloc(sizeof(struct Stack_));
    newStack->entries = new_stack_p(free_entries);
    newStack->curLabel = NULL;
    newStack->curFrame = NULL;
    return newStack;
}
void free_Stack(Stack stack)
{
    if(stack) {
        free_stack_p(stack->entries);
    }
    free(stack);
}
void push_Label(Stack stack, Label label)
{
    stack->curLabel = label;
    stack_push(stack->entries, label);
#ifndef NDEBUG
    printf("Push label of function %u\n", label_get_funcAddr(label));
#endif
}

void push_Frame(Stack stack, Frame frame)
{
    stack->curFrame = frame;
    stack->curLabel = NULL;
    stack_push(stack->entries, frame);
#ifndef NDEBUG
    printf("Push frame of module %s\n", frame->moduleInst->name);
#endif
}

void push_Value(Stack stack, Value* value)
{
#ifndef NDEBUG
    switch (value->type) {
        case Value_i32:
            printf("Push value i32 %d\n", value->value.i32);
            break;
        case Value_i64:
            printf("Push value i64 %ld\n", value->value.i64);
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
    stack_push(stack->entries, value);
}

int pop_Label(Stack stack, Label* label, _Bool restoreValue)
{
    if(!stack->curLabel) {
        return -1;
    }
    stack_p results = NULL;
    vector_p resultTypes = NULL;
    if(restoreValue) {
        // Save results
        results = new_stack_p(NULL);
        resultTypes = label_get_resultTypes(stack->curLabel);
        if(resultTypes) {
            for(uint32_t i = 0; i < vector_size(resultTypes); ++i) {
                Value* result = NULL;
                if(pop_Value(stack, &result)) {
                    return -3;
                }
                stack_push(results, result);
            }
        }
    }
    // Pop label
    for(Entry* entry = stack_top(Entry*, stack->entries); entry != NULL; entry = stack_top(Entry*, stack->entries)) {
        if(entry->entryType == Entry_Label) {
            *label = stack_pop(Label, stack->entries);
            break;
        } else if(entry->entryType == Entry_Value) {
            Value* value = NULL;
            pop_Value(stack, &value);
            free_Value(value);
        } else {
            stack->curLabel = NULL;
            return -2;
        }
    }

    // Restore results
    if(restoreValue) {
        if(resultTypes) {
            for(uint32_t i = 0; i < vector_size(resultTypes); ++i) {
                ValueType* type = vector_at(ValueType*, resultTypes, i);
                Value* result = stack_pop(Value*, results);
                if(result->type != *type) {
                    free(result);
                    return -4;
                }
                push_Value(stack, result);
            }
        }
        free_stack_p(results);
    }
    // Update curlabel
    for(stack_iterator it = stack_head(stack->entries); it != NULL; it = stack_iterator_next(it)) {
        Entry* entry = stack_iterator_get(Entry*, it);
        switch (entry->entryType) {
            case Entry_Label:
#ifndef NDEBUG
                printf("Pop label of function %u\n", label_get_funcAddr(*label));
#endif
                stack->curLabel = (Label)entry;
                return 0;
            case Entry_Frame:
#ifndef NDEBUG
                printf("Pop label of function %u\n", label_get_funcAddr(*label));
#endif
                stack->curLabel = NULL;
                return 0;
            default:
                break;
        }
    }
#ifndef NDEBUG
    printf("Pop label of function %u\n", label_get_funcAddr(*label));
#endif
    stack->curLabel = NULL;
    return 0;
}

int pop_Frame(Stack stack, Frame* framePtr, Label lastLabel)
{
    if(!stack->curFrame) {
        return -1;
    }
    stack_p values = new_stack_p(free_Value);
    Label label = lastLabel;
    for(stack_iterator it = stack_head(stack->entries); it != NULL; it = stack_head(stack->entries)) {
        Entry* entry = stack_iterator_get(Entry*,it);
        if(entry->entryType == Entry_Frame) {
            *framePtr = stack_pop(Frame, stack->entries);
            break;
        } else if(entry->entryType == Entry_Value) {
            Value* value = NULL;
            pop_Value(stack, &value);
            stack_push(values, value);
        } else if(entry->entryType == Entry_Label) {
            if(label) {
                free_Label(label);
            }
            pop_Label(stack, &label, 0);
        } else {
            stack->curFrame = NULL;
            return -2;
        }
    }
    if(label && label_get_resultTypes(label)) {
        size_t resultCount = vector_size(label_get_resultTypes(label));
        for(stack_iterator it = stack_head(values); it != NULL; it = stack_head(values)) {
            Value* retValue = stack_pop(Value*, values);
            if(stack_size(values) > resultCount) {
                free_Value(retValue);
            } else {
                push_Value(stack, retValue);
            }
        }
    }
    free_Label(label);
    free_stack_p(values);
    stack->curLabel = NULL;
    for(stack_iterator it = stack_head(stack->entries); it != NULL; it = stack_iterator_next(it)) {
        Entry* entry = stack_iterator_get(Entry*,it);
        switch (entry->entryType) {
            case Entry_Label:
                if(!stack->curLabel) {
                    stack->curLabel = (Label)entry;
                }
                break;
            case Entry_Frame:
#ifndef NDEBUG
                printf("Pop frame of module %s\n", (*framePtr)->moduleInst->name);
#endif
                stack->curFrame = (Frame)entry;
                return 0;
            default:
                break;
        }
    }
    stack->curFrame = NULL;
#ifndef NDEBUG
    printf("Pop frame of module %s\n", (*framePtr)->moduleInst->name);
#endif
    return 0;
}

int pop_Value(Stack stack, Value** value)
{
    Entry* cur = stack_top(Entry*, stack->entries);
    if(cur && (cur->entryType == Entry_Value)) {
        *value = stack_pop(Value*, stack->entries);
#ifndef NDEBUG
        switch ((*value)->type) {
            case Value_i32:
                printf("Pop value i32 %d\n", (*value)->value.i32);
                break;
            case Value_i64:
                printf("Pop value i64 %ld\n", (*value)->value.i64);
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

Label stack_cur_label(Stack stack)
{
    return stack->curLabel;
}

Frame stack_cur_frame(Stack stack)
{
    return stack->curFrame;
}

Entry* stack_top_entry(Stack stack)
{
    return stack_top(Entry*, stack->entries);
}