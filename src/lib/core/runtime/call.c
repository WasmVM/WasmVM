#include <core/Runtime.h>

#include <dataTypes/list_p.h>
#include <dataTypes/Frame_.h>
#include <dataTypes/Value.h>
#include <dataTypes/Label.h>
#include <instance/FuncInst.h>

int runtime_call(Stack* stack, Store* store, ControlInstrInst *control)
{
    Frame frame = new_Frame(stack->curFrame->moduleInst);
    uint32_t address = *(uint32_t*)frame->moduleInst->funcaddrs->at(frame->moduleInst->funcaddrs, *(uint32_t*)control->indices->at(control->indices, 0));
    FuncInst* func = (FuncInst*)store->funcs->at(store->funcs, address);
    // Pop parameters from Stack and append to local
    for(uint32_t i = 0; i < func->type->params->length; ++i) {
        ValueType* type = func->type->params->at(func->type->params, i);
        Value* value = NULL;
        pop_Value(stack, &value);
        frame->locals->push_back(frame->locals, value);
    }
    // Append private locals to local
    for(uint32_t i = 0; i < func->locals->length; ++i) {
        ValueType* type = func->locals->at(func->locals, i);
        switch (*type) {
            case Value_i32:
                frame->locals->push_back(frame->locals, new_i32Value(0));
                break;
            case Value_i64:
                frame->locals->push_back(frame->locals, new_i64Value(0));
                break;
            case Value_f32:
                frame->locals->push_back(frame->locals, new_f32Value(0));
                break;
            case Value_f64:
                frame->locals->push_back(frame->locals, new_f64Value(0));
                break;
            default:
                break;
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