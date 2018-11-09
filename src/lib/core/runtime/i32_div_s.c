#include <core/Runtime.h>
#include <dataTypes/Value.h>
#include <stdio.h>
#include <stdlib.h>

int runtime_i32_div_s(Stack* stack){
    Value *value1 = NULL, *value2 = NULL;
    stack->entries->pop(stack->entries, (void**)&value2); //Divisor
    stack->entries->pop(stack->entries, (void**)&value1); //Dividend

    // Error testing: divided by zero and integer overflow.
    if(value2->value.i32 == 0) {
        fprintf(stderr, "Divided by zero!");
        free(value1);
        free(value2);
        return (-1);
    } else if((value1->value.i32 == 0x80000000) && (value2->value.i32 == 0xFFFFFFFF)) {
        fprintf(stderr, "Integer overflow!");
        free(value1);
        free(value2);
        return (-1);
    } else {
        stack->entries->push(stack->entries, new_i32Value(value1->value.i32 / value2->value.i32));
        free(value1);
        free(value2);
        return 0;
    }
}
