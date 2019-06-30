#include <core/Runtime.h>

#include <stdlib.h>
#include <math.h>

int runtime_f64_sub(Stack stack)
{
    Value* operand1 = NULL;
    Value* operand2 = NULL;
    pop_Value(stack, &operand2);
    pop_Value(stack, &operand1);
    if(isnan(operand1->value.f64) || isnan(operand2->value.f64)) {
        push_Value(stack, new_f64Value(nan("")));
    } else if(isinf(operand1->value.f64) || isinf(operand2->value.f64)) {
        if(isinf(operand1->value.f64) && isinf(operand2->value.f64) && !(signbit(operand1->value.f64) ^ signbit(operand2->value.f64))) {
            push_Value(stack, new_f64Value(nan("")));
        } else if((isinf(operand1->value.f64) && !signbit(operand1->value.f64)) || (isinf(operand2->value.f64) && signbit(operand2->value.f64))) {
            push_Value(stack, new_f64Value(strtod("INF", NULL)));
        } else {
            push_Value(stack, new_f64Value(-strtod("INF", NULL)));
        }
    } else if(operand1->value.f64 == 0 && operand2->value.f64 == 0) {
        if(signbit(operand1->value.f64) ^ signbit(operand2->value.f64)) {
            push_Value(stack, new_f64Value(operand1->value.f64));
        } else {
            push_Value(stack, new_f64Value(+0.0));
        }
    } else {
        push_Value(stack, new_f64Value(operand1->value.f64 - operand2->value.f64));
    }
    free_Value(operand1);
    free_Value(operand2);
    return 0;
}
