#include <core/Runtime.h>
#include <dataTypes/Value.h>
#include <stdlib.h>

/*
 * if value_1 <= value_2, put 1 into stack, otherwise, put 0.
 */
int runtime_i32_le_s(Stack* stack)
{
    Value *value_1 = NULL, *value_2 = NULL;
    stack->entries->pop(stack->entries, (void**)&value_2);
    stack->entries->pop(stack->entries, (void**)&value_1);
    stack->entries->push(stack->entries,
                         new_i32Value(value_1->value.i32 <= value_2->value.i32));

    free(value_1);
    free(value_2);
    return 0;
}
