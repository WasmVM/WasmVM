#include <core/Runtime.h>
#include <dataTypes/Value.h>
#include <stddef.h>
#include <stdlib.h>

int runtime_i32_shr_u(Stack* stack)
{
    Value *value1 = NULL;
    Value *value2 = NULL;

    stack->entries->pop(stack->entries, (void **)&value2);
    stack->entries->pop(stack->entries, (void **)&value1);

    stack->entries->push(stack->entries, new_i32Value((uint32_t)value1->value.i32 >> (uint32_t)value2->value.i32));

    free(value1);
    free(value2);

    return 0;
}
