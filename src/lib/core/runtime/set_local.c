#include <core/Runtime.h>

#include <dataTypes/Value.h>
#include <core/Stack.h>

int runtime_set_local(Stack stack, uint32_t index)
{
    Value* operand = NULL;
    pop_Value(stack, &operand);
    Value* local = vector_at(Value*, frame_get_locals(stack_cur_frame(stack)), index);
    switch (local->type) {
        case Value_i32:
            local->value.i32 = operand->value.i32;
            break;
        case Value_i64:
            local->value.i64 = operand->value.i64;
            break;
        case Value_f32:
            local->value.f32 = operand->value.f32;
            break;
        case Value_f64:
            local->value.f64 = operand->value.f64;
            break;
        default:
            free_Value(operand);
            return -1;
    }
    free_Value(operand);
    return 0;
}