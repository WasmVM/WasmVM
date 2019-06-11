#include <core/Runtime.h>
#include <dataTypes/Value.h>
#include <stdlib.h>
#include <stdio.h>

int runtime_i32_rem_s(Stack stack)
{
    Value *value1 = NULL, *value2 = NULL;
    pop_Value(stack,&value2); //Divisor
    pop_Value(stack,&value1); //Dividend

    // Error detection: divided by zero
    if (value2->value.u32 == 0) {
        fprintf(stderr, "Divided by zero!");
        free(value1);
        free(value2);
        return -1;
    } else {
        push_Value(stack,
                   new_i32Value(value1->value.i32 % value2->value.i32));
        free(value1);
        free(value2);
        return 0;
    }
}