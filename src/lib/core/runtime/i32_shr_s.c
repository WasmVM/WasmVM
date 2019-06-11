#include <core/Runtime.h>
#include <dataTypes/Value.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>

int runtime_i32_shr_s(Stack stack)
{
    Value *value1 = NULL;
    Value *value2 = NULL;

    pop_Value(stack,&value2);
    pop_Value(stack,&value1);

    push_Value(stack, new_i32Value(value1->value.i32 >> value2->value.i32));

    free(value1);
    free(value2);


    return 0;
}
