#include <core/Runtime.h>
#include <dataTypes/Value.h>
#include <stdio.h>

int runtime_i32_rem_u(Stack* stack)
{
    Value *value1 = NULL, *value2 = NULL;
    stack->entries->pop(stack->entries, (void**)&value2); //Divisor
    stack->entries->pop(stack->entries, (void**)&value1); //Dividend

    // Error detection: divided by zero
    if (value2->value.u32 == 0) {
        fprintf(stderr, "Divided by zero!");
        free(value1);
        free(value2);
        return -1;
    } else {
        stack->entries->push(stack->entries,
                             new_i32Value(value1->value.u32 % value2->value.u32));
        free(value1);
        free(value2);
        return 0;
    }
}
