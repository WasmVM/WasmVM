#include <core/Runtime.h>
#include <dataTypes/Value.h>

int runtime_i64_const(Stack stack, int64_t value)
{
    push_Value(stack, new_i64Value(value));
    return 0;
}
