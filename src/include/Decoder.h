#ifndef DECODER
#define DECODER

#include <stdint.h>
#include <Stage.h>
#include <structures/WasmModule.h>

typedef struct _decoder {
    Stage       parent;         // inherit from Stage           
    uint8_t     magic[4];          // binary: magic number
    uint8_t     version[4];        // binary: version
    uint8_t*    wasm_source;    // source of input wasm file (uint8_t array)
    char*       module_name;    

    // (*setInput)(struct _decoder* decoder, char* input); // loading wasm source file (with specifying filename)
    // (*run)(struct _decoder* decoder);                   // parsing the raw wasm bytes into WasmModule, return string list (e.g. )
    // (*getOutput)(struct _decoder* decoder);             // get WasmModule from target decoder
} Decoder;

Decoder* new_Decoder();
void free_Decoder(Decoder* decoder);

#endif