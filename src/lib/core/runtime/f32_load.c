#include <core/Runtime.h>
#include <dataTypes/Value.h>
#include <instance/MemInst.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

int runtime_f32_load(Stack stack, MemInst* memory, uint32_t offset, uint32_t align)
{
    Value *value1 = NULL;
    pop_Value(stack, &value1);
    uint32_t ea = (uint32_t) value1->value.u32 + offset;

    if (ea + (32 / 8) > vector_size(memory->data)) {
        fprintf(stderr, "over the memory range! \n");
        free(value1);
        return -1;
    }

    float *data = (float *) (vector_data(uint8_t*, memory->data) + ea);
    push_Value(stack, new_f32Value(*data));
    free(value1);

    return 0;
}
