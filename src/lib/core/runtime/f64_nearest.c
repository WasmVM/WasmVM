#include <core/Runtime.h>

#include <math.h>

int runtime_f64_nearest(Stack stack)
{
    Value* value1 = NULL;
    pop_Value(stack, &value1);
    if(!isnan(value1->value.f64) && !isinf(value1->value.f64) && !(value1->value.f64 == +0.0 || value1->value.f64 == -0.0)) {
        if(value1->value.f64 <= 0.5 && value1->value.f64 > 0.0) {
            value1->value.f64 = +0.0;
        } else if(value1->value.f64 >= -0.5 && value1->value.f64 < -0.0) {
            value1->value.f64 = -0.0;
        } else {
            double diff = fabs(value1->value.f64 - floor(value1->value.f64));
            if(diff > 0.5) {
                value1->value.f64 = ceil(value1->value.f64);
            } else if(diff < 0.5) {
                value1->value.f64 = floor(value1->value.f64);
            } else if(ceil(value1->value.f64) / 2.0 == 0.0) {
                value1->value.f64 = ceil(value1->value.f64);
            } else {
                value1->value.f64 = floor(value1->value.f64);
            }
        }
    }
    push_Value(stack, value1);
    return 0;
}
