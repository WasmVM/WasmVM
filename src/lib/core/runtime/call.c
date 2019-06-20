#include <core/Runtime.h>

#include <dataTypes/list_p.h>
#include <dataTypes/Frame_.h>
#include <dataTypes/Value.h>
#include <dataTypes/Label.h>
#include <instance/FuncInst.h>

int runtime_call(Stack stack, Store store, ControlInstrInst *control)
{
    if(stack_cur_label(stack)) {
        label_set_instrIndex(stack_cur_label(stack), label_get_instrIndex(stack_cur_label(stack)) + 1);
    }
    Frame frame = new_Frame(stack_cur_frame(stack)->moduleInst);
    uint32_t address = *vector_at(uint32_t*, frame->moduleInst->funcaddrs, *vector_at(uint32_t*, control->indices, 0));
    FuncInst* func = vector_at(FuncInst*, store->funcs, address);
    // Pop parameters from Stack and append to local
    for(uint32_t i = 0; i < vector_size(func->type->params); ++i) {
        ValueType* type = vector_at(ValueType*, func->type->params, i);
        Value* value = NULL;
        pop_Value(stack, &value);
        vector_push_back(frame->locals, value);
        free_Value(value);
    }
    // Append private locals to local
    for(uint32_t i = 0; i < vector_size(func->locals); ++i) {
        ValueType* type = vector_at(ValueType*, func->locals, i);
        Value* value = NULL;
        switch (*type) {
            case Value_i32:
                value = new_i32Value(0);
                break;
            case Value_i64:
                value = new_i64Value(0);
                break;
            case Value_f32:
                value = new_f32Value(0);
                break;
            case Value_f64:
                value = new_f64Value(0);
                break;
            default:
                break;
        }
        if(value) {
            vector_push_back(frame->locals, value);
            free_Value(value);
        }
    }
    // Push frame to Stack
    push_Frame(stack, frame);
    // Execute block
    Label label = new_Label(address, 0, (list_size(func->code) > 0) ? list_size(func->code) - 1 : 0);
    label_set_resultTypes(label, func->type->results);
    push_Label(stack, label);
    return 0;
}