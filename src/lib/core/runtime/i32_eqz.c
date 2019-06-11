#include <core/Runtime.h>
#include <dataTypes/Value.h>
#include <stdlib.h>

int runtime_i32_eqz(Stack stack)
{
    Value *value1 = NULL;
    pop_Value(stack,&value1);
    push_Value(stack, new_i32Value(value1->value.i32 == 0));
    free(value1);
    return 0;
}
