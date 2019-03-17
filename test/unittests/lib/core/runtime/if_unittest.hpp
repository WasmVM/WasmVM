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
    Label* currentLabel = (Label*)malloc(sizeof(Label));
    currentLabel->parent.entryType = Entry_Label;
    currentLabel->funcAddr = 0;
    currentLabel->instrIndex = 1;
    currentLabel->contInstr = 2;

    ControlInstrInst* currentControl = new_ControlInstrInst();
    currentControl->elseAddr = 3;
    currentControl->endAddr = 4;

    push_Label(stack, currentLabel);

    // Check
    Label* result = NULL;
    push_Value(stack, new_i32Value(0));
    runtime_if(stack, currentControl);
    pop_Label(stack, &result);
    // Expect label's instrIndex to be ControlInstrInst's elseAddr.
    EXPECT_EQ(result->instrIndex, 3);
    // curLabel should be updated to the latest label's function address.
    EXPECT_EQ(result->funcAddr, 0);
    EXPECT_EQ(result->contInstr, 4);
    free(result);

    push_Value(stack, new_i32Value(1));
    runtime_if(stack, currentControl);
    pop_Label(stack, &result);
    EXPECT_EQ(result->instrIndex, 1);
    EXPECT_EQ(result->funcAddr, 0);
    EXPECT_EQ(result->contInstr, 4);
    free(result);

    // clean
    free_Stack(stack);
    free_ControlInstrInst(currentControl);
}
