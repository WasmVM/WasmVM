#include <core/Runtime.h>

#include <stdlib.h>

int runtime_i32_reinterpret_f32(Stack stack)
{
    Value* operand = NULL;
    pop_Value(stack, &operand);
    push_Value(stack, new_i32Value(operand->value.i32));
    free(operand);
    return 0;
}
