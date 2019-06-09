#ifndef WASMVM_DATATYPES_FRAME_DEF
#define WASMVM_DATATYPES_FRAME_DEF

#include <dataTypes/vector_p.h>
#include <instance/ModuleInst.h>

typedef struct Frame_ * Frame;

Frame new_Frame(ModuleInst* moduleInst);
void clean_Frame(Frame frame);
void free_Frame(Frame frame);
ModuleInst* frame_get_module(Frame frame);
vector_p frame_get_locals(Frame frame);

#endif