#include <core/Runtime.h>

#include <stdlib.h>
#include <math.h>

int runtime_f64_promote_f32(Stack stack)
{
    Value* operand = NULL;
    pop_Value(stack, &operand);
    if(isnan(operand->value.f32)) {
        push_Value(stack, new_f64Value(nan("")));
    } else if(isinf(operand->value.f32)) {
        push_Value(stack, new_f64Value(strtod("INF", NULL) * (-1 + (operand->value.f32 > 0) * 2)));
    } else if(operand->value.f32 == 0) {
        push_Value(stack, new_f64Value(+0 * (-1 + (operand->value.f32 > 0) * 2)));
    } else {
        push_Value(stack, new_f64Value((double) operand->value.f32));
    }
    free(operand);
    return 0;
}
