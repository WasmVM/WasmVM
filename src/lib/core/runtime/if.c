#include <core/Runtime.h>
#include <dataTypes/Value.h>
#include <dataTypes/Label.h>
#include <instance/ControlInstrInst.h>

int runtime_if(Stack *stack, Label *label, ControlInstrInst *control)
{
    Value *value = NULL;
    // Pop out the value to determine whether it should jump.
    stack->entries->pop(stack->entries, (void**)&value);

    // Jump to else address if the constant value is zero.
    if(value->value.i32 == 0)
    {
      label->instrIndex = control->elseAddr;
    }

    // Push the latast label to stack.
    stack->entries->push_label(stack->entries, label);
    free(value);
}
