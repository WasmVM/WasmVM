#include <core/Runtime.h>
#include <dataTypes/Value.h>
#include <stdlib.h>
#include <math.h>

int runtime_f32_abs(Stack stack)
{
    Value* value1 = NULL;
    pop_Value(stack, &value1);
    value1->value.f32 = copysignf(value1->value.f32, +0.0f);
    push_Value(stack, value1);
    return 0;
}
