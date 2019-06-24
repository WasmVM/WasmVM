#include <core/Runtime.h>
#include <stdlib.h>
#include <core/Store.h>
#include <dataTypes/stack_p.h>
#include <dataTypes/vector_p.h>
#include <dataTypes/Value.h>
#include <dataTypes/Label.h>
#include <dataTypes/FuncType.h>
#include <dataTypes/Frame.h>
#include <instance/FuncInst.h>

int runtime_return(Stack stack, Store store)
{
    Frame frame = NULL;
    if(pop_Frame(stack, &frame, NULL)) {
        return -1;
    }
    free_Frame(frame);
    if(stack_cur_label(stack)) {
        label_set_instrIndex(stack_cur_label(stack), label_get_instrIndex(stack_cur_label(stack)) + 1);
    }
    return 0;
}
