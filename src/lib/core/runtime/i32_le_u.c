#include <core/Runtime.h>
#include <dataTypes/Value.h>
#include <stdlib.h>

/*
 * if value_1 <= value_2, put 1 into stack, otherwise, put 0.
 */
int runtime_i32_le_u(Stack stack)
{
    Value *value_1 = NULL, *value_2 = NULL;
    pop_Value(stack,&value_2);
    pop_Value(stack,&value_1);
    push_Value(stack,
               new_i32Value(value_1->value.u32 <= value_2->value.u32));

    free(value_1);
    free(value_2);
    return 0;
}
