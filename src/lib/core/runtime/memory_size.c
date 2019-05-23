#include <core/Runtime.h>
#include <dataTypes/Value.h>
#include <stdint.h>

int memory_size(Stack* stack, MemInst* memory)
{
    size_t vec_length = memory->data->length;
    size_t vec_unitSize = memory->data->unitSize;
    uint32_t mem_data = vec_length * vec_unitSize;
    uint32_t pageSize = 64 * 1024;
    push_Value(stack, new_i32Value((int32_t)(mem_data - 1) / pageSize + 1));
    return 0;
}
