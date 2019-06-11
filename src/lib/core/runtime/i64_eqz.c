#include <core/Runtime.h>
#include <dataTypes/Value.h>
#include <stddef.h>

int runtime_i64_eqz(Stack stack)
{
    Value *value1 = NULL;
    if(pop_Value(stack,&value1)) return -1;
    push_Value(stack, new_i64Value(value1->value.i64 == 0));
    free_Value(value1);
    return 0;
}
