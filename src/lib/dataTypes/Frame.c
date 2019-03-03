#include "dataTypes/Frame.h"
#include <stdint.h>
#include <stdlib.h>

Frame* new_Frame()
{
    Frame *frame = (Frame*) malloc(sizeof(Frame));
    frame->localVals = new_vector(sizeof(uint64_t), NULL);
    frame->moduleInst = new_ModuleInst();

    return frame;
}

void clean_Frame(Frame *frame)
{
    free(frame->localVals);
    free(frame->moduleInst);
}

void free_Frame(Frame *frame)
{
    clean_Frame(frame);
}
