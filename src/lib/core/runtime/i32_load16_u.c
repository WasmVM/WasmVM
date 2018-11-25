#include <core/Runtime.h>
#include <dataTypes/Value.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

int runtime_i32_load16_u(Stack* stack, MemInst* memory, uint32_t offset, uint32_t align)
{
    Value *value1 = NULL;
<<<<<<< HEAD
    stack->entries->pop(stack->entries, (void **)&value1);
    uint32_t ea = (uint32_t)value1->value.i32 + offset;
    if(ea >= memory->max * 65536) {
=======
    int32_t *data = NULL;
    uint32_t ea = 0;
    stack->entries->pop(stack->entries, (void **)&value1);
    ea = (uint32_t)value1->value.i32 + offset;

    if(ea >= mems->length) {
>>>>>>> c259522ff04e7107b315996f860113e4a53ff190
        fprintf(stderr, "over the memory range! \n");
        free(value1);
        return -1;
    }
<<<<<<< HEAD
    uint16_t* data = (uint16_t*)(memory->data->data + ea);
    stack->entries->push(stack->entries, new_i32Value(*data));
    free(value1);
=======
    
    data = mems->at(mems, ea);
    *data = *data & (0xffff);
    stack->entries->push(stack->entries, new_i32Value(*data));
    free(value1);
    free(data);
>>>>>>> c259522ff04e7107b315996f860113e4a53ff190
    return 0;
}
