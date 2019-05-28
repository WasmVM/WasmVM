#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <core/Runtime.h>
#include <dataTypes/Value.h>
#include <dataTypes/Label.h>
#include <instance/ControlInstrInst.h>
}
#undef _Bool

SKYPAT_F(Runtime_control_if, valid)
{
    Stack* stack = new_Stack();

    // Label: {Entry, funcAddr, instrIndex, contInstr}
    Label* currentLabel = new_Label(0, 1, 2);

    ControlInstrInst* currentControl = new_ControlInstrInst();
    currentControl->endAddr = 4;

    push_Label(stack, currentLabel);

    // Check
    Label* result = NULL;
    runtime_loop(stack, currentControl);
    pop_Label(stack, &result);
    // Expect label's instrIndex to be ControlInstrInst's contAddr.
    EXPECT_EQ(result->instrIndex, 2);
    // curLabel should be updated to the latest label's function address.
    EXPECT_EQ(result->funcAddr, 0);
    EXPECT_EQ(result->contInstr, 2);
    EXPECT_EQ(result->endInstr, 4);
    free(result);

    // clean
    free_Stack(stack);
    free_ControlInstrInst(currentControl);
}
