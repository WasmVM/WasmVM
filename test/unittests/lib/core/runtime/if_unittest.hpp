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
    Stack* stack = new_Stack((void (*)(void*))free_Value);

    // Label: {Entry, funcAddr, instrIndex, contInstr}
    Label* currentLabel;
    currentLabel->funcAddr = 0x0000000;
    currentLabel->instrIndex = 0x00000001;
    currentLabel->contInstr = 0x00000002;

    ControlInstrInst* currentControl = new_ControlInstrInst();
    currentControl->elseAddr = 0x00000003;
    currentControl->endAddr = 0x00000004;

    Value *check_value = new_i32Value(0);
    stack->entries->push(stack->entries, check_value);

    runtime_if(stack, currentLabel, currentControl);

    // Expect label's instrIndex to be ControlInstrInst's elseAddr.
    EXPECT_EQ(currentLabel->instrIndex, 0x00000003);

    // curLabel should be updated to the latest label's function address.
    EXPECT_EQ(stack->curLabel, 0x00000000);

    // clean
    free_Value(check_value);
    free_Stack(stack);
    free_ControlInstrInst(currentControl);
}
