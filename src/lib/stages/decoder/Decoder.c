#include "Decoder.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dataTypes/FuncType.h>
#include <dataTypes/Value.h>
#include "sections.h"

static void setInput(Decoder* decoder, char* input)
{
    // store filename
    decoder->parent.input = (void*) input;
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
    uint8_t* wasm_source = (uint8_t*) malloc(file_len * sizeof(uint8_t));
    fread(wasm_source, file_len, 1, fin);
    fclose(fin);

    // start decoding process
    uint8_t *read_p = wasm_source;
    uint8_t *end_p = wasm_source + file_len - 1;

    // - magic number & version
    if(parse_magic_version(newModule, &read_p) < 0) {
        return -1;
    }

    // Section 1: Type
    if(parse_type_section(newModule, &read_p, &end_p) < 0) {
        return -1;
    }
    // Section 2: Import
    if(parse_import_section(newModule, &read_p, &end_p) < 0) {
        return -1;
    }
    /*
        // Section 3: Function
        if(skip_to_section(3, &read_p, &end_p) == 3) {
            // Parse "Function" Section
            if(parse_func_section(newModule, &read_p, &end_p) < 0) {
                return -1;
            }
        }
        // Section 4: Table
        if(skip_to_section(4, &read_p, &end_p) == 4) {
            // Parse "Table" Section
            if(parse_table_section(newModule, &read_p, &end_p) < 0) {
                return -1;
            }
        }
        // Section 5: Memory
        if(skip_to_section(5, &read_p, &end_p) == 5) {
            // Parse "Memory" Section
            if(parse_memory_section(newModule, &read_p, &end_p) < 0) {
                return -1;
            }
        }
        // Section 6: Global
        if(skip_to_section(6, &read_p, &end_p) == 6) {
            // Parse "Global" Section
            if(parse_global_section(newModule, &read_p, &end_p) < 0) {
                return -1;
            }
        }
        // Section 7: Export
        if(skip_to_section(7, &read_p, &end_p) == 7) {
            // Parse "Export" Section
            if(parse_export_section(newModule, &read_p, &end_p) < 0) {
                return -1;
            }
        }
        // Section 8: Start
        if(skip_to_section(8, &read_p, &end_p) == 8) {
            // Parse "Start" Section
            if(parse_start_section(newModule, &read_p, &end_p) < 0) {
                return -1;
            }
        }
        // Section 9: Element
        if(skip_to_section(9, &read_p, &end_p) == 9) {
            // Parse "Element" Section
            if(parse_element_section(newModule, &read_p, &end_p) < 0) {
                return -1;
            }
        }
        // Section 10: Code
        if(skip_to_section(10, &read_p, &end_p) == 10) {
            // Parse "Code" Section
            if(parse_code_section(newModule, &read_p, &end_p) < 0) {
                return -1;
            }
        }
        // Section 11: Data
        if(skip_to_section(11, &read_p, &end_p) == 11) {
            // Parse "Data" Section
            if(parse_data_section(newModule, &read_p, &end_p) < 0) {
                return -1;
            }
        }
    */
    free(wasm_source);
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