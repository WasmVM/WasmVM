#include "utils.h"

#include <stdio.h>

static void print_value(Value* value){
    switch (value->type){
    case Value_i32:
        printf("(i32) %d", value->value.i32);
        break;
    case Value_i64:
        printf("(i64) %ld", value->value.i64);
        break;
    case Value_f32:
        printf("(f32) %f", value->value.f32);
        break;
    case Value_f64:
        printf("(f64) %lf", value->value.f64);
        break;
    default:
        break;
    }
}

void _dump_stack(wasm_stack stack){
    for(u32_t i = 0; stack; ++i, stack = stack->next){
        printf("#%u ", i);
        switch (stack->type){
        case Entry_value:
            printf("[Value] ");
            print_value(&(stack->entry.value));
            printf("\n");
            break;
        case Entry_frame:
            printf("[Frame] %p arity: %u, last: %p", stack, stack->entry.frame.arity, stack->entry.frame.last);
            printf("\n");
            break;
        case Entry_label:
            printf("[Label] %p arity: %u, last: %p", stack, stack->entry.label.arity, stack->entry.label.last);
            printf("\n");
            break;
        default:
            break;
        }
    }
}