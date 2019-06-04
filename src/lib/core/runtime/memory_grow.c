#include <core/Runtime.h>
#include <dataTypes/Value.h>
#include <stdint.h>
#include <stdio.h>
#include <stddef.h>

int memory_grow(Stack* stack, MemInst* memory)
{
    const uint32_t pageSize = 64 * 1024;
    uint32_t sz = memory->data->length / pageSize;

    // check stacktop value type i32
    Value *value = stack_top(Value*, stack->entries);
    if(value->type != Value_i32) {
        fprintf(stderr, "memory grow by error value type!\n");
        return -1;
    }

    // pop data from stack
    pop_Value(stack, &value);

    // check memory max of pages and grow memory
    int32_t n = value->value.i32;
    if(n + sz <= memory->max) {
        memory->data->resize(memory->data, (n + sz) * pageSize);
    }

    // check result
    if(memory->data->length == sz * pageSize || memory->data == NULL) {
        push_Value(stack, new_i32Value(-1));
    } else {
        push_Value(stack, new_i32Value(sz));
    }

    return 0;
}
