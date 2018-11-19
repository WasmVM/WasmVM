#include <core/Runtime.h>
#include <dataTypes/Value.h>
#include <stdlib.h>

int runtime_i32_eqz(Stack* stack)
{
    Value *value1 = NULL;
    stack->entries->pop(stack->entries, (void**)&value1);
    stack->entries->push(stack->entries, new_i32Value(value1->value.i32 == 0));
    free(value1);
    return 0;
}
