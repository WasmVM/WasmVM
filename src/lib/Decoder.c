#include <dataTypes/FuncType.h>
#include <dataTypes/Value.h>
#include <Decoder.h>
#include "Utils.h"

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

char skip_to_section(uint8_t sectionNum, char* ptr, const char* endAddr)
{
    if(ptr > endAddr) {
        return -1;
    }
    // Check section code, and move ptr to the target
    while(*ptr < sectionNum) {
        ptr++;
        // Get section size
        uint32_t size = getLeb128_u32(ptr, endAddr);
        ptr += size;
    }
    char ret = *ptr;
    if(ret == sectionNum) {
        getLeb128_u32(++ptr, endAddr);
    }

    return ret;
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
    memcpy(decoder->magic, read_p, 4);
    if(*(uint32_t*) decoder->magic != toLittle32(WASM_MAGIC, 0)) {
        printf("%s isn't a legal wasm file.\n", decoder->module_name);
        // return error code
        return -1;
    }
    memcpy(decoder->version, read_p+4, 4);
    if(*(uint32_t*) decoder->version != toLittle32(WASM_VERSION, 0)) {
        printf("Wrong version of %s.\n", decoder->module_name);
        // return error code
        return -1;
    }
    *read_p+=8;

    // Module process ...
    uint32_t importedFuncCount = 0;
    // Section 1: Type
    if(skip_to_section(1, read_p, end_p) == 1) {
        uint32_t typeNum = getLeb128_u32(read_p, end_p);
        // Get all types
        while(typeNum-- > 0) {
            // FuncType init
            FuncType* newType = (FuncType*) malloc(sizeof(FuncType));
            newType->params = new_vector(sizeof(ValueType), (void(*)(void*))free);
            newType->results = new_vector(sizeof(ValueType), (void(*)(void*))free);
            if(*read_p != TYPE_Func) {
                printf("%s : Function type must start with function type code. (Wrong wasm)\n", decoder->module_name);
                return -1;
            }
            // Params
            uint32_t paramNum = getLeb128_u32(++read_p, end_p);
            while(paramNum-- > 0) {
                switch(*read_p) {
                    case TYPE_i32:
                        newType->params->push_back(newType->params, Value_i32);
                        break;
                    case TYPE_i64:
                        newType->params->push_back(newType->params, Value_i64);
                        break;
                    case TYPE_f32:
                        newType->params->push_back(newType->params, Value_f32);
                        break;
                    case TYPE_f64:
                        newType->params->push_back(newType->params, Value_f64);
                        break;
                    default:
                        printf("%s : No such parameter type.\n", decoder->module_name);
                        return -1;
                }
                read_p++;
            }
            // Results
            uint32_t resultNum = getLeb128_u32(read_p, end_p);
            while (resultNum-- > 0) {
                switch (*read_p) {
                    case TYPE_i32:
                        newType->results->push_back(newType->results, Value_i32);
                        break;
                    case TYPE_i64:
                        newType->results->push_back(newType->results, Value_i32);
                        break;
                    case TYPE_f32:
                        newType->results->push_back(newType->results, Value_i32);
                        break;
                    case TYPE_f64:
                        newType->results->push_back(newType->results, Value_i32);
                        break;
                    default:
                        printf("%s : No such parameter type.\n", decoder->module_name);
                        return -1;
                }
                read_p++;
            }
            // Push into newModule
            newModule->types->push_back(newModule->types,newType);
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