#include <core/Runtime.h>
#include <dataTypes/Value.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

int runtime_i32_load(Stack* stack, vector* mems, uint32_t offset, uint32_t align)
{
    Value *value1 = NULL;
    int32_t *data = NULL;
    uint32_t ea = 0;

    stack->entries->pop(stack->entries, (void **)&value1);
    ea = (uint32_t)value1->value.i32 + offset;

    data = mems->at(mems, ea);

    /**
     * vector <i32> return value should not be NULL.
     */
    if(data == NULL) {
        fprintf(stderr, "over the memory range!");
        free(value1);
        free(data);
        return -1;
    }

    stack->entries->push(stack->entries, new_i32Value(*data));

    free(value1);
    free(data);

    return 0;
}
