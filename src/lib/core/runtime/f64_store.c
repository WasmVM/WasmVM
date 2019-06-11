#include <core/Runtime.h>
#include <dataTypes/Value.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

int runtime_f64_store(Stack stack, MemInst* memory, uint32_t offset, uint32_t align)
{
    Value *value1 = NULL;
    Value *value2 = NULL;
    double *dataPointer = NULL;
    uint32_t ea = 0;

    // pop data form stack
    pop_Value(stack, &value2);
    pop_Value(stack, &value1);

    // check memory range
    ea = (uint32_t) value1->value.u32 + offset;
    if (ea + (64 / 8) > vector_size(memory->data)) {
        fprintf(stderr, "over the memory range! \n");
        free_Value(value1);
        free_Value(value2);
        return -1;
    }

    // store data to memory
    dataPointer = (double*)(vector_data(uint8_t*, memory->data) + ea);
    *dataPointer = value2->value.f64;

    // free object
    free(value1);
    free(value2);

    return 0;
}
