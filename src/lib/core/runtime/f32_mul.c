#include <core/Runtime.h>

#include <stdlib.h>
#include <math.h>

int runtime_f32_mul(Stack stack)
{
    Value* operand1 = NULL;
    Value* operand2 = NULL;
    pop_Value(stack, &operand2);
    pop_Value(stack, &operand1);
    if(isnan(operand1->value.f32) || isnan(operand2->value.f32)) {
        push_Value(stack, new_f32Value(nanf("")));
    } else if(isinf(operand1->value.f32) || isinf(operand2->value.f32)) {
        if(operand1->value.f32 == 0 || operand2->value.f32 == 0) {
            push_Value(stack, new_f32Value(nanf("")));
        } else if(signbit(operand1->value.f32) ^ signbit(operand2->value.f32)) {
            push_Value(stack, new_f32Value(-strtof("INF", NULL)));
        } else {
            push_Value(stack, new_f32Value(strtof("INF", NULL)));
        }
    } else if(operand1->value.f32 == 0 && operand2->value.f32 == 0) {
        if(signbit(operand1->value.f32) ^ signbit(operand2->value.f32)) {
            push_Value(stack, new_f32Value(-0.0f));
        } else {
            push_Value(stack, new_f32Value(+0.0f));
        }
    } else {
        push_Value(stack, new_f32Value(operand1->value.f32 * operand2->value.f32));
    }
    free_Value(operand1);
    free_Value(operand2);
    return 0;
}
