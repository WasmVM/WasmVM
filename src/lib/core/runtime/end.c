#include <core/Runtime.h>
#include <stdlib.h>
#include <core/Store.h>
#include <core/Stack_.h>
#include <dataTypes/stack_p.h>
#include <dataTypes/Value.h>
#include <dataTypes/Label.h>
#include <dataTypes/FuncType.h>
#include <dataTypes/Frame.h>
#include <instance/FuncInst.h>

int runtime_end(Stack stack, Store store)
{
    // Count label
    Label label = NULL;
    if(pop_Label(stack, &label, 1)) {
        return -1;
    }
    if(stack_cur_label(stack) == NULL) {
        Frame frame = NULL;
        if(pop_Frame(stack, &frame, label)) {
            return -2;
        }
        free_Frame(frame);
    } else {
        label_set_instrIndex(stack_cur_label(stack), label_get_contInstr(label));
        free_Label(label);
    }
    return 0;
}
