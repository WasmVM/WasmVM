#include <core/Runtime.h>
#include <stdlib.h>
#include <dataTypes/Value.h>
#include <dataTypes/Label.h>
#include <instance/ControlInstrInst.h>

int runtime_else(Stack *stack)
{
    label_set_instrIndex(stack->curLabel, label_get_contInstr(stack->curLabel));
    return 0;
}
