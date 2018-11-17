#include <dataTypes/FuncType.h>
#include <dataTypes/Value.h>
#include <Decoder.h>

#include "stages/decoder/sections.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void setInput(Decoder* decoder, char* input)
{
    // store filename
    decoder->module_name = input;
}

static WasmModule* getOutput(Decoder* decoder)
{
    return decoder->parent.output;
}

static int run(Decoder* decoder)
{
    // allocate WasmModule
    WasmModule* newModule = (WasmModule *) malloc(sizeof(WasmModule));
    decoder->parent.output = (void*) newModule;
    newModule->module_name = decoder->module_name;

    // read from input (read bytes)
    uint32_t file_len;
    FILE *fin = fopen(decoder->module_name, "rb");
    fseek(fin, 0, SEEK_END);
    file_len = ftell(fin);
    rewind(fin);
    decoder->wasm_source = (uint8_t*) malloc(file_len*sizeof(uint8_t));
    fread(decoder->wasm_source, file_len, 1, fin);
    fclose(fin);

    // start decoding process
    uint8_t *read_p = decoder->wasm_source;
    uint8_t *end_p = &decoder->wasm_source[file_len - 1];

    // - magic number & version
    if(parse_magic_version(newModule, &read_p) < 0) {
        return -1;
    }

    // record number of imported function
    uint32_t importedFuncCount = 0;
    // Section 1: Type
    if(skip_to_section(1, &read_p, &end_p) == 1) {
        // Parse "Type" Section, and then store the value into newModule
        if(parse_type_section(newModule, &read_p, &end_p) < 0) {
            return -1;
        }
    }
    // Section 2: Import
    if(skip_to_section(2, &read_p, &end_p) == 2) {
        // Parse "Import" Section
        if(parse_import_section(newModule, &importedFuncCount, &read_p, &end_p) < 0) {
            return -1;
        }
    }

    return 0;
}

Decoder* new_Decoder(Loader* loader)
{
    Decoder* newDecoder = (Decoder*) malloc(sizeof(Decoder));
    /* Attributes */
    newDecoder->loader = loader;
    /* Member function */
    newDecoder->parent.setInput = (void(*)(Stage*, void*))setInput;
    newDecoder->parent.getOutput = (void*(*)(Stage*))getOutput;
    newDecoder->parent.run = (int(*)(Stage*))run;

    return newDecoder;
}

void free_Decoder(Decoder* thisDecoder)
{
    free(thisDecoder->loader);
    free(thisDecoder);
}