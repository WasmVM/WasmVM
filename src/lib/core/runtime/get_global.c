#include <core/Runtime.h>

#include <dataTypes/Value.h>
#include <instance/GlobalInst.h>
#include <core/Stack.h>
#include <core/Store.h>

int runtime_get_global(Stack stack, Store store, uint32_t index)
{
    GlobalInst* global = vector_at(GlobalInst*, store->globals, *vector_at(uint32_t*, frame_get_module(stack_cur_frame(stack))->globaladdrs, index));
    switch (global->value.type) {
        case Value_i32:
            push_Value(stack, new_i32Value(global->value.value.i32));
            break;
        case Value_i64:
            push_Value(stack, new_i64Value(global->value.value.i64));
            break;
        case Value_f32:
            push_Value(stack, new_f32Value(global->value.value.f32));
            break;
        case Value_f64:
            push_Value(stack, new_f64Value(global->value.value.f64));
            break;
        default:
            return -1;
    }
    return 0;
}