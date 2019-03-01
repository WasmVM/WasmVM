#include <core/Runtime.h>
#include <stddef.h>
#include <dataTypes/Value.h>

int runtime_i64_popcnt(Stack* stack)
{
    Value *value1 = NULL;
    if( stack->entries->pop(stack->entries, (void**)&value1)) return -1;

    uint64_t value = value1->value.i64;
    uint8_t count = 0;
    while(value) {
        value &= value-1;
        count++;
    }
    stack->entries->push(stack->entries, new_i64Value(count));
    free_Value(value1);
    return 0;
}
