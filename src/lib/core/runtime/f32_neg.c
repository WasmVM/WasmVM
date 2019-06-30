#include <core/Runtime.h>
#include <dataTypes/Value.h>
#include <stdlib.h>

int runtime_f32_neg(Stack stack)
{
    Value* value1 = NULL;
    pop_Value(stack,&value1);
    value1->value.f32 = -value1->value.f32;
    push_Value(stack, value1);
    return 0;
}
