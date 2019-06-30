#include <core/Runtime.h>
#include <dataTypes/Value.h>
#include <stdlib.h>
#include <math.h>

int runtime_f64_abs(Stack stack)
{
    Value* value1 = NULL;
    pop_Value(stack, &value1);
    value1->value.f64 = copysign(value1->value.f64, +0.0);
    push_Value(stack, value1);
    return 0;
}
