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
    Stack stack = new_Stack();

    // Label: {Entry, funcAddr, instrIndex, contInstr}
    Label currentLabel = new_Label(0, 1, 2);

    ControlInstrInst* currentControl = new_ControlInstrInst();
    currentControl->endAddr = 4;

    push_Label(stack, currentLabel);

    // Check
    Label result = NULL;
    runtime_loop(stack, currentControl);
    pop_Label(stack, &result, 1);
    EXPECT_EQ(label_get_instrIndex(result), 2);
    EXPECT_EQ(label_get_funcAddr(result), 0);
    EXPECT_EQ(label_get_contInstr(result), 1);
    EXPECT_EQ(label_get_endInstr(result), 4);
    free(result);

    // clean
    free_Stack(stack);
    free_ControlInstrInst(currentControl);
}
