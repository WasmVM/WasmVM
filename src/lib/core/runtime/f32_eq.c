#include <core/Runtime.h>

int runtime_f32_eq(Stack stack)
{
    Value* operand1 = NULL;
    Value* operand2 = NULL;

    pop_Value(stack, &operand2);
    pop_Value(stack, &operand1);

    push_Value(stack, new_f32Value(operand1->value.f32 == operand2->value.f32));

    free_Value(operand1);
    free_Value(operand2);
    return 0;
}
