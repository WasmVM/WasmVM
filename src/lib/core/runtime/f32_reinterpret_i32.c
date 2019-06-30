#include <core/Runtime.h>

#include <stdlib.h>

int runtime_f32_reinterpret_i32(Stack stack)
{
    Value* operand = NULL;
    pop_Value(stack, &operand);
    push_Value(stack, new_f32Value(operand->value.f32));
    free(operand);
    return 0;
}
