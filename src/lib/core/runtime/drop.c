#include <core/Runtime.h>
#include <dataTypes/Entry.h>
#include <dataTypes/Value.h>
#include <stddef.h>
#include <stdlib.h>

int runtime_drop(Stack stack)
{
    Value *value = NULL;
    if(pop_Value(stack,&value)) return -1;
    free_Value(value);
    return 0;
}
