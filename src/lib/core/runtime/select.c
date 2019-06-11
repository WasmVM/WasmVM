#include <core/Runtime.h>
#include <dataTypes/Value.h>
#include <stddef.h>

int runtime_select(Stack stack)
{
    Value *value1, *value2, *value3;
    int res = 0;
    res = pop_Value(stack,&value3);
    if(res) {
        return res;
    }
    res = pop_Value(stack,&value2);
    if(res) {
        return res;
    }
    res = pop_Value(stack,&value1);
    if(res) {
        return res;
    }
    if(value3->value.i32) {
        push_Value(stack, value1);
        free_Value(value2);
    } else {
        push_Value(stack, value2);
        free_Value(value1);
    }
    free_Value(value3);
    return res;
}
