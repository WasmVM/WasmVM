#include <core/Runtime.h>
#include <dataTypes/Value.h>
#include <stdlib.h>

int runtime_f32_abs(Stack stack)
{
    Value* value1 = NULL;
    pop_Value(stack, &value1);
    value1->value.u32 &= 0x7fffffff;
    value1->type = Value_f32;
    push_Value(stack, value1);
    return 0;
}
