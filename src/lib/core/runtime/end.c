#include <core/Runtime.h>
#include <stdlib.h>
#include <core/Store.h>
#include <dataTypes/stack.h>
#include <dataTypes/Value.h>
#include <dataTypes/Label.h>
#include <dataTypes/FuncType.h>
#include <dataTypes/Frame.h>
#include <instance/FuncInst.h>

int runtime_end(Stack *theStack, Store* store)
{
    Label* label = NULL;
    if(pop_Label(theStack, &label)) {
        return -1;
    }
    if(theStack->curLabel == NULL) {
        FuncType type = ((FuncInst*)store->funcs->at(store->funcs, label->funcAddr))->type;
        stack* valStack = new_stack(NULL);
        for(uint32_t i = 0; i < type->results->length; ++i) {
            Value* retValue = NULL;
            pop_Value(theStack, &retValue);
            valStack->push(valStack, retValue);
        }
        Frame frame = NULL;
        if(pop_Frame(theStack, &frame)) {
            return -2;
        }
        for(uint32_t i = 0; i < type->results->length; ++i) {
            ValueType* resultType = (ValueType*)type->results->at(type->results, i);
            Value* retValue = NULL;
            valStack->pop(valStack, (void**)&retValue);
            push_Value(theStack, retValue);
        }
        free_stack(valStack);
        free_Frame(frame);
    }
    if(theStack->curLabel) {
        theStack->curLabel->instrIndex = label->contInstr;
    }
    free_Label(label);
    return 0;
}
