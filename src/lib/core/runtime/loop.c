#include <core/Runtime.h>
#include <stdlib.h>
#include <dataTypes/Label.h>
#include <instance/ControlInstrInst.h>

int runtime_loop(Stack *stack, ControlInstrInst *control)
{
    Label* label = NULL;
    label = new_Label(stack->curLabel->funcAddr, stack->curLabel->instrIndex + 1, stack->curLabel->instrIndex + 1);
    label->endInstr = control->endAddr;
    label->resultTypes = control->resultTypes;
    push_Label(stack, label);
    return 0;
}
