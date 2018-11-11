#include <core/Runtime.h>
#include <dataTypes/Value.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>

int runtime_i32_shr_s(Stack* stack)
{
    int32_t temp = 0;
    Value *value1 = NULL;
    Value *value2 = NULL;

    stack->entries->pop(stack->entries, (void **)&value2);
    stack->entries->pop(stack->entries, (void **)&value1);

    temp = value1->value.i32;

    temp = (temp >= 0)
           ? temp >> value2->value.i32
           : ~((~temp) >> value2->value.i32);

    stack->entries->push(stack->entries, new_i32Value(temp));

    free(value1);
    free(value2);


    return 0;
}
