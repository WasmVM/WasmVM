#include <core/Runtime.h>
#include <dataTypes/Value.h>
#include <stddef.h>

int runtime_select(Stack* stack)
{
    Value *value1, *value2, *value3;
    unsigned int check_size = stack->entries->size;
    if(check_size < 3) return -1;
    pop_Value(stack,&value3);
    pop_Value(stack,&value2);
    pop_Value(stack,&value1);
    if(value3->value.i32) {
        push_Value(stack, value1);
        free_Value(value2);
    } else {
        push_Value(stack, value2);
        free_Value(value1);
    }
    free_Value(value3);
    return 0;
}
