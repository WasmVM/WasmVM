#include <core/Runtime.h>

#include <stdlib.h>

int runtime_f32_convert_s_i32(Stack stack)
{
    Value* operand = NULL;
    pop_Value(stack, &operand);
    push_Value(stack, new_f32Value((float)operand->value.i32));
    free(operand);
    return 0;
}
