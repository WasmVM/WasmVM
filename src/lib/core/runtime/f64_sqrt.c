#include <core/Runtime.h>

#include <math.h>

int runtime_f64_sqrt(Stack stack)
{
    Value* value1 = NULL;
    pop_Value(stack, &value1);
    value1->value.f64 = sqrt(value1->value.f64);
    push_Value(stack, value1);
    return 0;
}
