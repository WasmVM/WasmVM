#include <core/Runtime.h>

#include <dataTypes/list_p.h>
#include <dataTypes/Frame_.h>
#include <dataTypes/Value.h>
#include <dataTypes/Label.h>
#include <instance/FuncInst.h>
#include <instance/TableInst.h>

int runtime_call_indirect(Stack stack, Store store, ControlInstrInst *control)
{
    if(stack_cur_label(stack)) {
        label_set_instrIndex(stack_cur_label(stack), label_get_instrIndex(stack_cur_label(stack)) + 1);
    }

    uint32_t tableAddr = *vector_at(uint32_t*, stack_cur_frame(stack)->moduleInst->tableaddrs, 0);
    TableInst* table = vector_at(TableInst*, store->tables, tableAddr);

    uint32_t typeIndex = *vector_at(uint32_t*, control->indices, 0);
    FuncType type = vector_at(FuncType,stack_cur_frame(stack)->moduleInst->types, typeIndex);

    Value* operand = NULL;
    if(pop_Value(stack, &operand)) {
        return -1;
    }
    if(operand->value.u32 >= vector_size(table->elem)) {
        return -2;
    }
    uint32_t address = *vector_at(uint32_t*, table->elem, operand->value.u32);
    FuncInst* func = vector_at(FuncInst*, store->funcs, address);
    if((vector_size(func->type->params) != vector_size(type->params)) || (vector_size(func->type->results) != vector_size(type->results))) {
        return -3;
    }
    for(size_t i = 0; i < vector_size(func->type->params); ++i) {
        if(*vector_at(ValueType*, func->type->params, i) != *vector_at(ValueType*, type->params, i)) {
            return -4;
        }
    }
    for(size_t i = 0; i < vector_size(func->type->results); ++i) {
        if(*vector_at(ValueType*, func->type->results, i) != *vector_at(ValueType*, type->results, i)) {
            return -5;
        }
    }

    Frame frame = new_Frame(func->module);
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