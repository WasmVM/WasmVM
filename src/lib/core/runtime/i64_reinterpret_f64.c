#include <core/Runtime.h>

#include <stdlib.h>

int runtime_i64_reinterpret_f64(Stack stack)
{
    Value* operand = NULL;
    pop_Value(stack, &operand);
    push_Value(stack, new_i64Value(operand->value.i64));
    free(operand);
    return 0;
}
