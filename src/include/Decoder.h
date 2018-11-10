#ifndef WASMVM_DECODER_DEF
#define WASMVM_DECODER_DEF

#include <stdint.h>
#include <Stage.h>
#include <Loader.h>
#include <structures/WasmModule.h>

#define WASM_MAGIC      0x6D736100  // led
#define WASM_VERSION    0x01

#define TYPE_i32 0x7F
#define TYPE_i64 0x7E
#define TYPE_f32 0x7D
#define TYPE_f64 0x7C
#define TYPE_Func 0x60
#define TYPE_Table_anyfunc 0x70

#define IMPORT_Func 0x00
#define IMPORT_Table 0x01
#define IMPORT_Mem 0x02
#define IMPORT_Global 0x03

typedef struct _decoder {
    Stage       parent;         // inherit from Stage           
    uint8_t     magic[4];       // binary: magic number
    uint8_t     version[4];     // binary: version
    uint8_t*    wasm_source;    // source of input wasm file (uint8_t array)
    char*       module_name;    // module name of input source
    Loader*     loader;

    // (*setInput)(struct _decoder* decoder, char* input); // loading wasm source file (with specifying filename)
    // (*run)(struct _decoder* decoder);                   // parsing the raw wasm bytes into WasmModule, return string list (e.g. )
    // (*getOutput)(struct _decoder* decoder);             // get WasmModule from target decoder
} Decoder;

Decoder* new_Decoder(Loader* loader); // pass Loader pointer to Decoder constructor
void free_Decoder(Decoder* decoder);

// Private member func 
char skip_to_section(uint8_t sectionNum, char* ptr, const char* endAddr);   // skip the current reading pointer to specified section


#endif