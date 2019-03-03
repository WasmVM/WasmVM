#include <core/Runtime.h>
#include <dataTypes/Value.h>
#include <stdio.h>

int runtime_i64_div_s(Stack* stack)
{
    Value *value1 = NULL, *value2 = NULL;
    stack->entries->pop(stack->entries, (void**)&value2); // Divisor
    stack->entries->pop(stack->entries, (void**)&value1); // Dividend

    // Error testing: divided by zero and integer overflow.
    if(value2->value.i64 == 0) {
        fprintf(stderr, "Divided by zero!\n");
        free_Value(value1);
        free_Value(value2);
        return -1;
    } else {
        stack->entries->push(stack->entries, new_i64Value(value1->value.i64 / value2->value.i64));
        free_Value(value1);
        free_Value(value2);
        return 0;
    }
}
