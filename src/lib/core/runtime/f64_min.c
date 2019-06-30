#include <core/Runtime.h>

#include <math.h>

int runtime_f64_min(Stack stack)
{
    Value* operand1 = NULL;
    Value* operand2 = NULL;

    pop_Value(stack, &operand2);
    pop_Value(stack, &operand1);

    push_Value(stack, new_f64Value(fmin(operand1->value.f64, operand2->value.f64)));

    free_Value(operand1);
    free_Value(operand2);
    return 0;
}
