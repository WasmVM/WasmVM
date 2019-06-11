#include <core/Runtime.h>
#include <dataTypes/Value.h>
#include <dataTypes/vector_p.h>
#include <stdint.h>

int runtime_memory_size(Stack stack, MemInst* memory)
{
    size_t vec_length = vector_size(memory->data);
    uint32_t pageSize = 64 * 1024;
    push_Value(stack, new_i32Value(vec_length / pageSize));
    return 0;
}
