#include <core/Runtime.h>

#include <math.h>

int runtime_f32_trunc(Stack stack)
{
    Value* value1 = NULL;
    pop_Value(stack, &value1);
    value1->value.f32 = truncf(value1->value.f32);
    push_Value(stack, value1);
    return 0;
}
