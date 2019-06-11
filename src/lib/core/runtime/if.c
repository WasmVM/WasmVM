#include <core/Runtime.h>
#include <stdlib.h>
#include <dataTypes/Value.h>
#include <dataTypes/Label.h>
#include <instance/ControlInstrInst.h>

int runtime_if(Stack stack, ControlInstrInst *control)
{
    Value *value = NULL;
    // Pop out the value to determine whether it should jump.
    pop_Value(stack, &value);

    Label label = NULL;

    // Jump to else address if the constant value is zero.
    if(value->value.i32 == 0) {
        label = new_Label(label_get_funcAddr(stack_cur_label(stack)), control->elseAddr + 1, control->endAddr);
    } else {
        label = new_Label(label_get_funcAddr(stack_cur_label(stack)), label_get_instrIndex(stack_cur_label(stack)) + 1, control->endAddr);
    }
    label_set_resultTypes(label, control->resultTypes);
    // Push the latast label to stack.
    push_Label(stack, label);
    free(value);
    return 0;
}
