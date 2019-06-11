#include <core/Runtime.h>
#include <stdlib.h>
#include <dataTypes/Value.h>
#include <dataTypes/Label.h>
#include <instance/ControlInstrInst.h>

int runtime_else(Stack stack)
{
    label_set_instrIndex(stack_cur_label(stack), label_get_contInstr(stack_cur_label(stack)));
    return 0;
}
