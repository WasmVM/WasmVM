#ifndef WASMVM_INSTANCE_FRAME_DEF
#define WASMVM_INSTANCE_FRAME_DEF

#include <dataTypes/vector.h>
#include <dataTypes/Value.h>
#include <instance/ModuleInst.h>

typedef struct {
    vector* locals;       // Value
    ModuleInst* moduleInst;
} Frame;

Frame* new_Frame(ModuleInst* moduleInst);
void clean_Frame(Frame *thisFrame);
void free_Frame(Frame *thisFrame);

#endif