#ifndef WASMVM_INSTANCE_FRAME_DEF
#define WASMVM_INSTANCE_FRAME_DEF

#include <dataTypes/vector.h>
#include <ModuleInst.h>

typedef struct {
    vector* vals;       // ValueType
    ModuleInst module_inst;
} Frame;

#endif