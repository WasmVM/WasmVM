#include <core/Runtime.h>

#include <stdint.h>
#include <dataTypes/Value.h>
#include <dataTypes/stack_p.h>
#include <dataTypes/Label.h>

int runtime_br_if(Stack stack, ControlInstrInst *control)
{
    Value* value = NULL;
    if(pop_Value(stack, &value)) {
        return -2;
    }
    if(value->value.i32) {
        return runtime_br(stack, control);
    } else {
        label_set_instrIndex(stack_cur_label(stack), label_get_instrIndex(stack_cur_label(stack)) + 1);
        return 0;
    }
}
