#include <core/Runtime.h>
#include <dataTypes/Value.h>
#include <stddef.h>

int runtime_select(Stack* stack)
{
    Value *value1, *value2, *value3;
    unsigned int check_size = stack->entries->size;
    if(check_size < 3) return -1;
    stack->entries->pop(stack->entries, (void**)&value3);
    stack->entries->pop(stack->entries, (void**)&value2);
    stack->entries->pop(stack->entries, (void**)&value1);
    if(value3->value.i32) {
        stack->entries->push(stack->entries, value1);
        free_Value(value2);
    } else {
        stack->entries->push(stack->entries, value2);
        free_Value(value1);
    }
    free_Value(value3);
    return 0;
}
