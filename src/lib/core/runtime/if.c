#include <core/Runtime.h>
#include <stdlib.h>
#include <dataTypes/Value.h>
#include <dataTypes/Label.h>
#include <instance/ControlInstrInst.h>

int runtime_if(Stack *stack, ControlInstrInst *control)
{
    Value *value = NULL;
    // Pop out the value to determine whether it should jump.
    stack->entries->pop(stack->entries, (void**)&value);

    Label* label = (Label*) malloc(sizeof(Label));
    label->parent.entryType = Entry_Label;
    label->funcAddr = stack->curLabel->funcAddr;

    // Jump to else address if the constant value is zero.
    if(value->value.i32 == 0) {
        label->instrIndex = control->elseAddr;
    } else {
        label->instrIndex = stack->curLabel->instrIndex;
    }
    label->contInstr = control->endAddr;

    // Push the latast label to stack.
    push_Label(stack, label);
    free(value);
    return 0;
}
