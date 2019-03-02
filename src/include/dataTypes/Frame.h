#ifndef WASMVM_INSTANCE_FRAME_DEF
#define WASMVM_INSTANCE_FRAME_DEF

#include <dataTypes/vector.h>
#include <ModuleInst.h>

typedef struct {
    vector* localVals;       // ValueType
    ModuleInst* module_inst;
} Frame;

Frame* new_frame(vector* localVals, ModuleInst* module);
void free_frame(Frame *thisFrame);

#endif