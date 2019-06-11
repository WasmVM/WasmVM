#include <core/Runtime.h>
#include <dataTypes/Value.h>
#include <stdlib.h>
#include <stddef.h>

int runtime_i32_shl(Stack stack)
{
    Value *value1 = NULL;
    Value *value2 = NULL;

    pop_Value(stack,&value2);
    pop_Value(stack,&value1);

    push_Value(stack, new_i32Value(value1->value.i32 << value2->value.i32));

    free(value1);
    free(value2);

    return 0;
}
