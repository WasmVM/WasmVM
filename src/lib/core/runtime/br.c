#include <core/Runtime.h>

#include <stdint.h>
#include <dataTypes/Value.h>
#include <dataTypes/stack_p.h>
#include <dataTypes/Label.h>

int runtime_br(Stack stack, ControlInstrInst *control)
{
    stack_p values = new_stack_p(NULL);
    Label label = NULL;
    for(uint32_t i = *vector_at(uint32_t*, control->indices, 0) + 1; i > 0;) {
        Entry* entry = stack_top_entry(stack);
        switch (entry->entryType) {
            case Entry_Value: {
                Value* value = NULL;
                pop_Value(stack, &value);
                stack_push(values, value);
            }
            break;
            case Entry_Label:
                i -= 1;
                if(label != NULL) {
                    free_Label(label);
                }
                pop_Label(stack, &label, 0);
                break;
            default:
                return -1;
                break;
        }
    }
    // Set InstrIndex
    if(!stack_cur_label(stack)) {
        Frame frame = NULL;
        pop_Frame(stack, &frame, NULL);
        if(frame) {
            free_Frame(frame);
        }
    } else {
        label_set_instrIndex(stack_cur_label(stack), label_get_endInstr(label) + 1);
    }
    if(label_get_resultTypes(label)) {
        for (size_t i = 0, resultCount = vector_size(label_get_resultTypes(label)); i < stack_size(values); ++i) {
            Value* value = stack_pop(Value*, values);
            if(i < resultCount) {
                push_Value(stack, value);
            } else {
                free_Value(value);
            }
        }
    }

    free_stack_p(values);
    free_Label(label);
    return 0;
}
