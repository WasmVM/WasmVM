#ifndef WASMVM_INSTANCE_FRAME_DEF
#define WASMVM_INSTANCE_FRAME_DEF

#include <dataTypes/vector.h>
#include <instance/ModuleInst.h>

typedef struct Frame_{
    vector* localVals;       // ValueType
    ModuleInst* moduleInst;
} Frame;

Frame* new_Frame();
void clean_Frame(Frame *thisFrame);
void free_Frame(Frame *thisFrame);

#endif