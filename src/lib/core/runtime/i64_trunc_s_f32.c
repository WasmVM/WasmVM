#include <core/Runtime.h>

int runtime_i64_trunc_s_f32(Stack stack)
{
    Value* operand = NULL;
    pop_Value(stack, &operand);
    push_Value(stack, new_i64Value((int64_t)operand->value.f32));
    free_Value(operand);
    return 0;
}
