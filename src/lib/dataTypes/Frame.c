#include "Frame_.h"

#include <stdint.h>
#include <stdlib.h>
#include <dataTypes/Value.h>

Frame new_Frame(ModuleInst* moduleInst)
{
    Frame frame = (Frame) malloc(sizeof(struct Frame_));
    frame->parent.entryType = Entry_Frame;
    frame->locals = new_vector_p(Value, NULL);
    frame->moduleInst = moduleInst;
    return frame;
}

void clean_Frame(Frame frame)
{
    free_vector_p(frame->locals);
}

void free_Frame(Frame frame)
{
    if(frame) {
        clean_Frame(frame);
        free(frame);
    }
}

ModuleInst* frame_get_module(Frame frame)
{
    return frame->moduleInst;
}

vector_p frame_get_locals(Frame frame)
{
    return frame->locals;
}