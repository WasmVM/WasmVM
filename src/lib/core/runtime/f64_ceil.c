#include <core/Runtime.h>

#include <math.h>

int runtime_f64_ceil(Stack stack)
{
    Value* value1 = NULL;
    pop_Value(stack, &value1);
    value1->value.f64 = ceil(value1->value.f64);
    push_Value(stack, value1);
    return 0;
}
