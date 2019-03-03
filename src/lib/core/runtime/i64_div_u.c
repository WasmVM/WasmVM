#include <core/Runtime.h>
#include <dataTypes/Value.h>
#include <stdio.h>
#include <stdlib.h>

int runtime_i64_div_u(Stack* stack)
{
    Value *value1 = NULL, *value2 = NULL;
    stack->entries->pop(stack->entries, (void**)&value2); // Divisor
    stack->entries->pop(stack->entries, (void**)&value1); // dividend

    // Error detection: divided by zero
    if (value2->value.i64 == 0) {
        fprintf(stderr, "Divided by zero!\n");
        free_Value(value1);
        free_Value(value2);
        return -1;
    } else {
        stack->entries->push(stack->entries, new_i64Value(value1->value.u64 / value2->value.u64));
        free_Value(value1);
        free_Value(value2);
        return 0;
    }
}
