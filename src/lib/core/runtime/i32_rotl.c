#include <core/Runtime.h>
#include <dataTypes/Value.h>
#include <stdlib.h>

int runtime_i32_rotl(Stack* stack){
    Value *value1 = NULL, *value2 = NULL;
    stack->entries->pop(stack->entries, (void**)&value2);
    stack->entries->pop(stack->entries, (void**)&value1);

    uint32_t ans = value1->value.i32, tmp = value1->value.i32, offset = value2->value.i32;
    ans >>= offset;
    tmp <<= (32 - offset);
    ans = tmp | ans;
    
    stack->entries->push(stack->entries, new_i32Value(ans));
    free(value1);
    free(value2);
    return 0;
}

