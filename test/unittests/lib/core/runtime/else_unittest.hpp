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
    Stack stack = new_Stack();

    // Label: {Entry, funcAddr, instrIndex, contInstr}
    Label currentLabel = new_Label(0, 4, 4);

    ControlInstrInst* currentControl = new_ControlInstrInst();
    currentControl->elseAddr = 3;
    currentControl->endAddr = 4;

    push_Label(stack, currentLabel);

    // Check
    Label result = NULL;
    runtime_else(stack);
    pop_Label(stack, &result, 1);
    // Expect label's instrIndex to be ControlInstrInst's endAddr.
    EXPECT_EQ(label_get_instrIndex(result), 4);
    // curLabel should be updated to the latest label's function address.
    EXPECT_EQ(label_get_funcAddr(result), 0);
    EXPECT_EQ(label_get_contInstr(result), 4);
    free(result);

    // clean
    free_Stack(stack);
    free_ControlInstrInst(currentControl);
}
