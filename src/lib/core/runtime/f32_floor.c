#include <core/Runtime.h>

#include <math.h>

int runtime_f32_floor(Stack stack)
{
    Value* value1 = NULL;
    pop_Value(stack, &value1);
    value1->value.f32 = floorf(value1->value.f32);
    push_Value(stack, value1);
    return 0;
}
