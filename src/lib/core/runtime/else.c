#include <core/Runtime.h>
#include <stdlib.h>
#include <dataTypes/Value.h>
#include <dataTypes/Label.h>
#include <instance/ControlInstrInst.h>

int runtime_else(Stack *stack, ControlInstrInst *control)
{
    stack->curLabel->instrIndex = control->endAddr;
    stack->curLabel->contInstr = control->endAddr;
    return 0;
}
