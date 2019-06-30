#include <core/Runtime.h>

#include <dataTypes/Value.h>
#include <core/Stack.h>

int runtime_get_local(Stack stack, uint32_t index)
{
    Value* local = vector_at(Value*, frame_get_locals(stack_cur_frame(stack)), index);
    switch (local->type) {
        case Value_i32:
            push_Value(stack, new_i32Value(local->value.i32));
            break;
        case Value_i64:
            push_Value(stack, new_i64Value(local->value.i64));
            break;
        case Value_f32:
            push_Value(stack, new_f32Value(local->value.f32));
            break;
        case Value_f64:
            push_Value(stack, new_f64Value(local->value.f64));
            break;
        default:
            return -1;
    }
    return 0;
}