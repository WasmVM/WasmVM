#include <core/Runtime.h>
#include <dataTypes/Value.h>
#include <stdlib.h>
#include <stddef.h>

int runtime_i32_shl(Stack* stack)
{
    Value *value1 = NULL;
    Value *value2 = NULL;

    stack->entries->pop(stack->entries, (void **)&value2);
    stack->entries->pop(stack->entries, (void **)&value1);

    stack->entries->push(stack->entries, new_i32Value(value1->value.i32 << value2->value.i32));

    free(value1);
    free(value2);

    return 0;
}
