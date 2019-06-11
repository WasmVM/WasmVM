#include "Decoder_.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <dataTypes/FuncType.h>
#include <dataTypes/Value.h>
#include <structures/WasmModule.h>
#include "sections.h"

static int run(Decoder decoder)
{
    // allocate WasmModule
    WasmModule* module = decoder->parent.output;

    // read from input (read bytes)
    uint32_t file_len;
    FILE *fin = fopen(module->module_name, "rb");
    fseek(fin, 0, SEEK_END);
    file_len = ftell(fin);
    rewind(fin);
    uint8_t* wasm_source = (uint8_t*) malloc(file_len * sizeof(uint8_t));
    fread(wasm_source, file_len, 1, fin);
    fclose(fin);

    // start decoding process
    uint8_t *read_p = wasm_source;
    uint8_t *end_p = wasm_source + file_len - 1;

    // magic number & version
    if(parse_magic_version(module, &read_p) < 0) {
        return -1;
    }
    // Section 1: Type
    if(parse_type_section(module, &read_p, end_p) < 0) {
        return -2;
    }
    // Section 2: Import
    if(parse_import_section(module, &read_p, end_p, decoder->loader, decoder->executor) < 0) {
        return -3;
    }
    // Section 3: Function
    if(parse_func_section(module, &read_p, end_p) < 0) {
        return -4;
    }
    // Section 4: Table
    if(parse_table_section(module, &read_p, end_p) < 0) {
        return -5;
    }
    // Section 5: Memory
    if(parse_memory_section(module, &read_p, end_p) < 0) {
        return -6;
    }
    // Section 6: Global
    if(parse_global_section(module, &read_p, end_p) < 0) {
        return -7;
    }
    // Section 7: Export
    if(parse_export_section(module, &read_p, end_p) < 0) {
        return -8;
    }
    // Section 8: Start
    if(parse_start_section(module, &read_p, end_p) < 0) {
        return -9;
    }
    // Section 9: Element
    if(parse_element_section(module, &read_p, end_p) < 0) {
        return -10;
    }
    // Section 10: Code
    if(parse_code_section(module, &read_p, end_p) < 0) {
        return -11;
    }
    // Section 11: Data
    if(parse_data_section(module, &read_p, end_p) < 0) {
        return -12;
    }
    free(wasm_source);
    return 0;
}

Decoder new_Decoder(Component* loader, Executor executor, WasmModule* output)
{
    Decoder newDecoder = (Decoder) malloc(sizeof(struct Decoder_));
    /* Attributes */
    newDecoder->loader = (Loader)loader;
    newDecoder->executor = executor;
    /* Member function */
    newDecoder->parent.input = NULL;
    newDecoder->parent.output = (void*)output;
    newDecoder->parent.run = (int(*)(Stage*))run;
    return newDecoder;
}

void free_Decoder(Decoder thisDecoder)
{
    free(thisDecoder->loader);
    free(thisDecoder);
}