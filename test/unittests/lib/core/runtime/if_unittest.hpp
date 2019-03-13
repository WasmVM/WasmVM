#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <core/Runtime.h>
#include <dataTypes/Value.h>
#include <dataTypes/Label.h>
#include <instance/ControlInstrInst.h>
}
#undef _Bool

SKYPAT_F(Runtime_control_if, regular)
{
    Stack* stack = new_Stack();

    // Label: {Entry, funcAddr, instrIndex, contInstr}
    Label* currentLabel;
    currentLabel->funcAddr = 0;
    currentLabel->instrIndex = 1;
    currentLabel->contInstr = 2;

    ControlInstrInst* currentControl = new_ControlInstrInst();
    currentControl->elseAddr = 3;
    currentControl->endAddr = 4;

    Value *check_value = new_i32Value(0);
    stack->entries->push(stack->entries, check_value);
    push_Label(stack, currentLabel);

    runtime_if(stack, currentControl);

    // Expect label's instrIndex to be ControlInstrInst's elseAddr.
    EXPECT_EQ(currentLabel->instrIndex, 3);

    // curLabel should be updated to the latest label's function address.
    EXPECT_EQ(stack->curLabel, 0);

    // clean
    free_Stack(stack);
    free_ControlInstrInst(currentControl);
}
