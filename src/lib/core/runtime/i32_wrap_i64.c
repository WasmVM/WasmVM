#include <core/Runtime.h>

#include <stdlib.h>

int runtime_i32_wrap_i64(Stack stack)
{
    Value* operand = NULL;
    pop_Value(stack, &operand);
    push_Value(stack, new_i32Value((int32_t)operand->value.i64));
    free(operand);
    return 0;
}
