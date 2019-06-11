#include <core/Runtime.h>
#include <stdlib.h>
#include <core/Store.h>
#include <dataTypes/stack_p.h>
#include <dataTypes/Value.h>
#include <dataTypes/Label.h>
#include <dataTypes/FuncType.h>
#include <dataTypes/Frame.h>
#include <instance/FuncInst.h>

int runtime_end(Stack stack, Store* store)
{
    Label label = NULL;
    if(pop_Label(stack, &label)) {
        return -1;
    }
    if(stack_cur_label(stack) == NULL) {
        FuncType type = (vector_at(FuncInst*, store->funcs, label_get_funcAddr(label)))->type;
        stack_p valStack = new_stack_p(NULL);
        for(uint32_t i = 0; i < vector_size(type->results); ++i) {
            Value* retValue = NULL;
            pop_Value(stack, &retValue);
            stack_push(valStack, retValue);
        }
        Frame frame = NULL;
        if(pop_Frame(stack, &frame)) {
            return -2;
        }
        for(uint32_t i = 0; i < vector_size(type->results); ++i) {
            ValueType* resultType = vector_at(ValueType*, type->results, i);
            Value* retValue = stack_pop(Value*, valStack);
            push_Value(stack, retValue);
        }
        free_stack_p(valStack);
        free_Frame(frame);
    }
    if(stack_cur_label(stack)) {
        label_set_instrIndex(stack_cur_label(stack), label_get_contInstr(label));
    }
    free_Label(label);
    return 0;
}
