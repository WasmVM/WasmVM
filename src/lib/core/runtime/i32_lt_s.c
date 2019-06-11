#include <core/Runtime.h>
#include <dataTypes/Value.h>
#include <stdlib.h>

int runtime_i32_lt_s(Stack stack)
{
    Value *value_1 = NULL, *value_2 = NULL;
    pop_Value(stack,&value_2);
    pop_Value(stack,&value_1);
    push_Value(stack,
               new_i32Value(value_1->value.i32 < value_2->value.i32));

    free(value_1);
    free(value_2);
    return 0;
}
