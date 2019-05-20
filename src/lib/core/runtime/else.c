#include <core/Runtime.h>
#include <stdlib.h>
#include <dataTypes/Value.h>
#include <dataTypes/Label.h>
#include <instance/ControlInstrInst.h>

int runtime_else(Stack *stack)
{
    stack->curLabel->instrIndex = stack->curLabel->contInstr;
    return 0;
}
