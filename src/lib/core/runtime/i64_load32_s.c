#include <core/Runtime.h>
#include <dataTypes/Value.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

int runtime_i64_load32_s(Stack stack, MemInst* memory, uint32_t offset, uint32_t align)
{
    Value *value1 = NULL;
    pop_Value(stack,&value1);

    uint32_t ea = value1->value.i32 + offset;

    if (ea + (64 / 8) > vector_size(memory->data)) {
        fprintf(stderr, "over the memory range!\n");
        free(value1);
        return -1;
    }

    int32_t *data = (int32_t*)(vector_data(uint8_t*, memory->data) + ea);

    push_Value(stack, new_i64Value((int64_t)*data));

    free(value1);
    return 0;
}
