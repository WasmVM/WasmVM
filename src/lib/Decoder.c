#include <Decoder.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void setInput(Decoder* decoder, char* input){
    // store filename 
    decoder->module_name = input;
}

static WasmModule* getOutput(Decoder* decoder){
    return decoder->parent.output;
}

static int run(Decoder* decoder){
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

    // - magic number & version
    memcpy(decoder->magic, read_p, 4);
    memcpy(decoder->version, read_p+4, 4);
    *read_p+=8;



    return 0;
}

Decoder* new_Decoder(){
    Decoder* newDecoder = (Decoder*) malloc(sizeof(Decoder));
    /* Attributes */

    /* Member function */
    newDecoder->parent.setInput = (void(*)(Stage*, void*))setInput;
    newDecoder->parent.getOutput = (void*(*)(Stage*))getOutput;
    newDecoder->parent.run = (int(*)(Stage*))run;
    
    return newDecoder;
}

void free_Decoder(Decoder* thisDecoder){
    free(thisDecoder);
}