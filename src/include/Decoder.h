#ifndef WASMVM_DECODER_DEF
#define WASMVM_DECODER_DEF

#include <stdint.h>
#include <Stage.h>
#include <Loader.h>
#include <structures/WasmModule.h>

typedef struct _decoder {
    Stage       parent;         // inherit from Stage
    uint8_t*    wasm_source;    // source of input wasm file (uint8_t array)
    char*       module_name;    // module name of input source
    Loader*     loader;
} Decoder;

Decoder* new_Decoder(Loader* loader); // pass Loader pointer to Decoder constructor
void free_Decoder(Decoder* decoder);

#endif