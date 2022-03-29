#ifndef WASMVM_INVOKE_INVOKE_
#define WASMVM_INVOKE_INVOKE_

#include <defines.h>
#include <Stack.h>
#include <Store.h>

void invoke(wasm_stack* stack, wasm_store store, u32_t funcaddr);
void execute(wasm_stack* stack, wasm_store store);

#endif