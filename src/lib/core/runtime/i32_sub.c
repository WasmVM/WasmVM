#include <core/Runtime.h>

int runtime_i32_sub(Stack* stack){
    Value *val1 = NULL, *val2 = NULL;
    stack->entries->pop(stack->entries, &val2);
    stack->entries->pop(stack->entries, &val1);
    Value *result = new_i32Value(val1->value.i32 - val2->value.i32);
    stack->entries->push(stack->entries, result);
    free(val1);
    free(val2);
    return 0;
}
