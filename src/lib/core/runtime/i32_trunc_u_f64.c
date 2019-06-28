#include <core/Runtime.h>

#include <stdlib.h>

int runtime_i32_trunc_u_f64(Stack stack)
{
    Value* operand = NULL;
    pop_Value(stack, &operand);
    push_Value(stack, new_i32Value((int32_t)operand->value.f64 * (-(operand->value.f64 < 0) * 2 + 1)));
    free(operand);
    return 0;
}
