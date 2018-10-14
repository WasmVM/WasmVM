#include <core/Runtime.h>

int runtime_i32_add(Stack* stack){
    Value *val1 = NULL, *val2 = NULL;
    stack->entries->pop(stack->entries, (void**)&val2);
    stack->entries->pop(stack->entries, (void**)&val1);
    stack->entries->push(stack->entries, new_i32Value(val1->value.i32 + val2->value.i32));
    return 0;
}
