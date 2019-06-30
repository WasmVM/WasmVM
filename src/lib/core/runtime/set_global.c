#include <core/Runtime.h>

#include <dataTypes/Value.h>
#include <instance/GlobalInst.h>
#include <core/Stack.h>
#include <core/Store.h>

int runtime_set_global(Stack stack, Store store, uint32_t index)
{
    GlobalInst* global = vector_at(GlobalInst*, store->globals, *vector_at(uint32_t*, frame_get_module(stack_cur_frame(stack))->globaladdrs, index));
    Value* operand = NULL;
    pop_Value(stack, &operand);
    switch (operand->type) {
        case Value_i32:
            global->value.value.i32 = operand->value.i32;
            break;
        case Value_i64:
            global->value.value.i64 = operand->value.i64;
            break;
        case Value_f32:
            global->value.value.f32 = operand->value.f32;
            break;
        case Value_f64:
            global->value.value.f64 = operand->value.f64;
            break;
        default:
            return -1;
    }
    free_Value(operand);
    return 0;
}