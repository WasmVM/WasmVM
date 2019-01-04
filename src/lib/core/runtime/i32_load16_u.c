#include <core/Runtime.h>
#include <dataTypes/Value.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>


int runtime_i32_load16_u(Stack* stack, MemInst* memory, uint32_t offset, uint32_t align)
{
    Value *value1 = NULL;
    stack->entries->pop(stack->entries, (void **)&value1);
    uint32_t ea = (uint32_t)value1->value.i32 + offset;
    if(ea >= memory->max * 65536) {
        fprintf(stderr, "over the memory range! \n");
        free(value1);
        return -1;
    }
    uint16_t* data = (uint16_t*)(memory->data->data + ea);
    stack->entries->push(stack->entries, new_i32Value(*data));
    free(value1);
    return 0;
}
