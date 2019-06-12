#include <core/Runtime.h>
#include <stdlib.h>
#include <dataTypes/Label.h>
#include <instance/ControlInstrInst.h>

int runtime_loop(Stack stack, ControlInstrInst *control)
{
    Label label = NULL;
    label = new_Label(label_get_funcAddr(stack_cur_label(stack)), label_get_instrIndex(stack_cur_label(stack)) + 1, label_get_instrIndex(stack_cur_label(stack)));
    label_set_endInstr(label, control->endAddr);
    label_set_resultTypes(label, control->resultTypes);
    push_Label(stack, label);
    return 0;
}
