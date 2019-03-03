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

    stack->entries->push(stack->entries, new_i32Value(value1->value.u32 >> value2->value.u32));

    free(value1);
    free(value2);

    return 0;
}
