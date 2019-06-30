#include <core/Runtime.h>

#include <math.h>

int runtime_f32_nearest(Stack stack)
{
    Value* value1 = NULL;
    pop_Value(stack, &value1);
    if(!isnan(value1->value.f32) && !isinf(value1->value.f32) && !(value1->value.f32 == +0.0f || value1->value.f32 == -0.0f)) {
        if(value1->value.f32 <= 0.5f && value1->value.f32 > 0.0f) {
            value1->value.f32 = +0.0f;
        } else if(value1->value.f32 >= -0.5f && value1->value.f32 < -0.0f) {
            value1->value.f32 = -0.0f;
        } else {
            float diff = fabsf(value1->value.f32 - floorf(value1->value.f32));
            if(diff > 0.5) {
                value1->value.f32 = ceilf(value1->value.f32);
            } else if(diff < 0.5) {
                value1->value.f32 = floorf(value1->value.f32);
            } else if(ceilf(value1->value.f32) / 2.0f == 0.0f) {
                value1->value.f32 = ceilf(value1->value.f32);
            } else {
                value1->value.f32 = floorf(value1->value.f32);
            }
        }
    }
    push_Value(stack, value1);
    return 0;
}
