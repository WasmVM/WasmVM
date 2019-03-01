#include <core/Runtime.h>
#include <dataTypes/Value.h>
#include <stdio.h>
#include <stdlib.h>

int runtime_i64_rem_s(Stack* stack)
{
    Value *value1 = NULL, *value2 = NULL;
    stack->entries->pop(stack->entries, (void**)&value2); // Divsor
    stack->entries->pop(stack->entries, (void**)&value1); // Dividend

    // Error detection: divided by zero
    if (value2->value.i64 == 0) {
        fprintf(stderr, "Divided by zero!\n");
        free_Value(value1);
        free_Value(value2);
        return -1;
    } else {
        stack->entries->push(stack->entries, new_i64Value(value1->value.i64 % value2->value.i64));
        free_Value(value1);
        free_Value(value2);
        return 0;
    }
}
