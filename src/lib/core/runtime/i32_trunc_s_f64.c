#include <core/Runtime.h>

#include <stdlib.h>

int runtime_i32_trunc_s_f64(Stack stack)
{
    Value* operand = NULL;
    pop_Value(stack, &operand);
    push_Value(stack, new_i32Value((int32_t)operand->value.f64));
    free(operand);
    return 0;
}
