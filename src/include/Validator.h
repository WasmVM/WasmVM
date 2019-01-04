#ifndef WASMVM_VALIDATOR_DEF
#define WASMVM_VALIDATOR_DEF

#include <Stage.h>
#include <structures/WasmModule.h>

typedef Stage Validator;

Validator* new_Validator(WasmModule* module);

#endif