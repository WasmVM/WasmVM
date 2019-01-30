#include <core/Runtime.h>
#include <dataTypes/Entry.h>
#include <stddef.h>
#include <dataTypes/Value.h>

int runtime_drop(Stack* stack)
{
    Value *value = NULL;
    int ans = stack->entries->pop(stack->entries, (void**)&value);
    return ans;
}
