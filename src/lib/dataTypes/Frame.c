#include "dataTypes/Frame.h"
#include <stdint.h>
#include <stdlib.h>

Frame* new_Frame(ModuleInst* moduleInst)
{
    Frame *frame = (Frame*) malloc(sizeof(Frame));
    frame->locals = new_vector(sizeof(Value), NULL);
    frame->moduleInst = moduleInst;
    return frame;
}

void clean_Frame(Frame *frame)
{
    free_vector(frame->locals);
}

void free_Frame(Frame *frame)
{
    clean_Frame(frame);
    free(frame);
}
