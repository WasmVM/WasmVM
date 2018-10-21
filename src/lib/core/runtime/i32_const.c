#include <core/Runtime.h>

int runtime_i32_const(Stack* stack, int32_t value1){
    stack->entries->push(stack->entries, new_i32Value(value1))
    return 0;
}
