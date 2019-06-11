#ifndef WASMVM_DECODER_DEF
#define WASMVM_DECODER_DEF

#include <Executor.h>
#include <Component.h>
#include <structures/WasmModule.h>

typedef struct Decoder_ * Decoder;

Decoder new_Decoder(Component* loader, Executor executor, WasmModule* output); // pass Loader pointer to Decoder constructor
void free_Decoder(Decoder decoder);

#endif