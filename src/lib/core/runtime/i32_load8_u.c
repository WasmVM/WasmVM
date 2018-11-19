#include <core/Runtime.h>
#include <dataTypes/Value.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

int runtime_i32_load8_u(Stack* stack, vector* mems, uint32_t offset, uint32_t align)
{
    Value *value1 = NULL;
    int32_t *data = NULL;
    uint32_t ea = 0;
    stack->entries->pop(stack->entries, (void **)&value1);
    ea = (uint32_t)value1->value.i32 + offset;
    if(ea >= mems->length) {
        fprintf(stderr, "over the memory range! \n");
        free(value1);
        return -1;
    }
    data = mems->at(mems, ea);
    *data = *data & (0xff);
    stack->entries->push(stack->entries, new_i32Value(*data));
    free(value1);
    free(data);
    return 0;
}