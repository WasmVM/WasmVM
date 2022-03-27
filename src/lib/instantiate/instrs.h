#ifndef WASMVM_INSTANCIATE_INSTRS_
#define WASMVM_INSTANCIATE_INSTRS_

#include <defines.h>
#include <Opcodes.h>
#include <instances/InstrInst.h>
#include <structures/WasmFunc.h>

size_t get_code_size(const WasmFunc* func);
void fill_func_body(const WasmFunc* func, byte_t* data);

#endif