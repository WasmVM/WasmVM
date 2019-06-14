#include <core/Runtime.h>

#include <stdint.h>
#include <dataTypes/stack_p.h>
#include <dataTypes/Label.h>

int runtime_br(Stack stack, ControlInstrInst *control)
{
    stack_p values = new_stack_p(NULL);
    Label lastLabel = NULL;
    for(uint32_t i = *vector_at(uint32_t*, control->indices, 0) + 1; i > 0;) {
        Entry* entry = stack_top_entry(stack);
        switch (entry->entryType) {
            case Entry_Value:
                stack_push(values, stack_pop_entry(stack));
                break;
            case Entry_Label:
                i -= 1;
                if(lastLabel != NULL) {
                    free_Label(lastLabel);
                }
                lastLabel = (Label)stack_pop_entry(stack);
                break;
            default:
                return -1;
                break;
        }
    }
    for (size_t i = 0, resultCount = vector_size(label_get_resultTypes(lastLabel)); i < stack_size(values); ++i) {
        Value* value = stack_pop(Value*, values);
        if(i < resultCount) {
            push_Value(stack, value);
        } else {
            free_Value(value);
        }
    }
    return 0;
}
