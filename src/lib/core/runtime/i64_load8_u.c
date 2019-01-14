#include <core/Runtime.h>
#include <dataTypes/Value.h>
#include <instance/MemInst.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

int runtime_i64_load8_u(Stack* stack, MemInst* memory, uint32_t offset, uint32_t align)
{
    Value *value1 = NULL;
    stack->entries->pop(stack->entries, (void **) &value1);
    uint64_t ea = (uint64_t) value1->value.i64 + offset;

    if (ea >= memory->max * 65536) {
        fprintf(stderr, "over the memory range! \n");
        free(value1);
        return -1;
    }

    uint8_t *data = (uint8_t *) (memory->data->data + ea);
    stack->entries->push(stack->entries, new_i64Value(*data));
    free(value1);

    return 0;
}
