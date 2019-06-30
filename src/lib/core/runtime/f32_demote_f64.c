#include <core/Runtime.h>

#include <stdlib.h>
#include <math.h>

int runtime_f32_demote_f64(Stack stack)
{
    Value* operand = NULL;
    pop_Value(stack, &operand);
    if(isnan(operand->value.f64)) {
        push_Value(stack, new_f32Value(nanf("")));
    } else if(isinf(operand->value.f64)) {
        push_Value(stack, new_f32Value(strtof("INF", NULL) * (-1 + (operand->value.f64 > 0) * 2)));
    } else if(operand->value.f64 == 0) {
        push_Value(stack, new_f32Value(+0 * (-1 + (operand->value.f64 > 0) * 2)));
    } else {
        push_Value(stack, new_f32Value((float) operand->value.f64));
    }
    free(operand);
    return 0;
}
