#include <core/Runtime.h>
#include <dataTypes/Value.h>
#include <stddef.h>

int runtime_i64_eqz(Stack* stack)
{
    Value *value1 = NULL;
    if(stack->entries->pop(stack->entries, (void**)&value1)) return -1;
    stack->entries->push(stack->entries, new_i64Value(value1->value.i64 == 0));
    free_Value(value1);
    return 0;
}
