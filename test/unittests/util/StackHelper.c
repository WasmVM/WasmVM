#include "StackHelper.h"

#include <core/Stack_.h>
#include <dataTypes/stack_p.h>

size_t stack_get_size(Stack stack)
{
    return stack_size(stack->entries);
}