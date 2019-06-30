#include <core/Runtime.h>

#include <stdlib.h>

int runtime_f64_reinterpret_i64(Stack stack)
{
    Value* operand = NULL;
    pop_Value(stack, &operand);
    push_Value(stack, new_f64Value(operand->value.f64));
    free(operand);
    return 0;
}
