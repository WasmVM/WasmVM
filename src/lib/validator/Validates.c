#include "Validates.h"

int validate_FunctionType(FuncType* funcType)
{
    return funcType->results->length <= 1; // TODO: Unleash later
}
