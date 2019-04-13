#ifndef WASMVM_DECODER_DEF
#define WASMVM_DECODER_DEF

#include <stdint.h>
#include <Stage.h>
#include <Loader.h>
#include <structures/WasmModule.h>

typedef struct _decoder {
    Stage       parent;         // inherit from Stage
    Loader*     loader;
    Store* store;
    vector* moduleInsts;
} Decoder;

Decoder* new_Decoder(Component* loader, Store* store, vector* moduleInsts); // pass Loader pointer to Decoder constructor
void free_Decoder(Decoder* decoder);

#endif