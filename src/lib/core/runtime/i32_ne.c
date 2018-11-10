#include <core/Runtime.h>
#include <dataTypes/Value.h>
#include <stdlib.h>

int runtime_i32_ne(Stack* stack){
    Value *value1 = NULL, *value2 = NULL;
    stack->entries->pop(stack->entries, (void**)&val2);
    stack->entries->pop(stack->entries, (void**)&val1);

    stack->entries->push(stack->entries, new_i32Value(val1->value.i32 != val2->value.i32));
    free(value1);
    free(value2);
    return 0;
}
