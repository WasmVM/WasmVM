#include <core/Runtime.h>

int runtime_i64_trunc_u_f64(Stack stack)
{
    Value* operand = NULL;
    pop_Value(stack, &operand);
    push_Value(stack, new_i64Value((int64_t)operand->value.f64 * (-(operand->value.f64 < 0) * 2 + 1)));
    free_Value(operand);
    return 0;
}
