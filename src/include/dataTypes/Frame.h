#ifndef WASMVM_INSTANCE_FRAME_DEF
#define WASMVM_INSTANCE_FRAME_DEF

#include <dataTypes/vector.h>
#include <ModuleInst.h>

typedef struct {
    vector* localVals;       // ValueType
    ModuleInst* module_inst;
} Frame;

//push the function arguments to localVals when calling new_frame
Frame* new_Frame(vector* localVals, ModuleInst* module);
void free_Frame(Frame *thisFrame);

#endif