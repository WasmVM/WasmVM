#include <core/Runtime.h>
#include <dataTypes/Value.h>

#include <stdlib.h>

int runtime_i64_extend_u_i32(Stack stack)
{
    Value *value1 = NULL;
    pop_Value(stack,&value1);
    push_Value(stack, new_i64Value((uint32_t)(value1->value.i32)));
    free(value1);
    return 0;
}
