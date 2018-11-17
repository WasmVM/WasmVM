#include <stages/decoder/sections.h>

#include "Utils.h"

#include <stdint.h>

char skip_to_section(uint8_t sectionNum, uint8_t **ptr, uint8_t **endAddr)
{
    if(*ptr > *endAddr) {
        return -1;
    }
    // Check section code, and move ptr to the target
    while(**ptr < sectionNum) {
        (*ptr)++;
        // Get section size
        uint32_t size = getLeb128_u32(ptr, endAddr);
        *ptr += size;
    }
    char ret = **ptr;
    if(ret == sectionNum) {
        getLeb128_u32(++(ptr), (endAddr));
    }

    return ret;
}

int parse_magic_version(WasmModule *module, uint8_t **read_p)
{
    if(*(uint32_t*) *read_p != toLittle32(WASM_MAGIC, 0)) {
        printf("%s isn't a legal wasm file.\n", module->module_name);
        // return error code
        return -1;
    }
    *read_p += 4;

    if(*(uint32_t*) *read_p != toLittle32(WASM_VERSION, 0)) {
        printf("Wrong version of %s.\n", module->module_name);
        // return error code
        return -1;
    }
    *read_p += 4;

    return 0;
}

int parse_type_section(WasmModule *newModule, uint8_t **read_p, uint8_t **end_p)
{
    uint32_t typeNum = getLeb128_u32(read_p, end_p);
    // Get all types
    while(typeNum-- > 0) {
        // FuncType init
        FuncType* newType = (FuncType*) malloc(sizeof(FuncType));
        newType->params = new_vector(sizeof(ValueType), (void(*)(void*))free);
        newType->results = new_vector(sizeof(ValueType), (void(*)(void*))free);
        if(**read_p != TYPE_Func) {
            printf("%s : Function type must start with function type code. (Wrong wasm)\n", newModule->module_name);
            return -1;
        }
        // Params
        uint32_t paramNum = getLeb128_u32(++read_p, end_p);

        while(paramNum-- > 0) {
            switch(**read_p) {
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
                    printf("%s : No such parameter type.\n", newModule->module_name);
                    return -1;
            }
            *read_p++;
        }

        // Results
        uint32_t resultNum = getLeb128_u32(read_p, end_p);
        while (resultNum-- > 0) {
            switch (**read_p) {
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
            *read_p++;
        }
        // Push into newModule
        newModule->types->push_back(newModule->types,newType);
    }

    return 0;
}

int parse_import_section(WasmModule *newModule, uint32_t *importedFuncCount, uint8_t **read_p, uint8_t **end_p)
{
    uint32_t importNum = getLeb128_u32(read_p, end_p);
    while(importNum-- > 0) {
        WasmImport *newImport = (WasmImport*)malloc(sizeof(WasmImport));
        // Get Module Name length
        uint32_t depNameLen = getLeb128_u32(read_p, end_p);
        // Get Module Name
        char depName[depNameLen + 1];
        depName[depNameLen] = '\0';
        strncpy(depName, *read_p, depNameLen);
        *read_p += depNameLen;
        newImport->module = depName;
        // Get Name length
        uint32_t nameLen = getLeb128_u32(read_p, end_p);
        // Get name
        char name[nameLen + 1];
        name[nameLen] = '\0';
        strncpy(name, *read_p, nameLen);
        *read_p += nameLen;
        newImport->name = name;
        // Load dependencies
        // FIXME: https://github.com/WasmVM/WasmVM/blob/Tiny-Syscall/src/Component/Loader.cpp#L165
        // Does it need to load the dependencies right now?
        // Or just pass the handler to Loader?

        // import kind
        // Follow & Reference:
        // - https://webassembly.github.io/spec/core/syntax/modules.html#syntax-index
        // - https://webassembly.github.io/spec/core/binary/modules.html#binary-importsec
        switch(*(*read_p++)) {
            case IMPORT_Func:
                newImport.descType = Desc_Func;
                newImport.descIdex = getLeb128_u32(read_p, end_p);
                // if exceed, then return with error
                /*if(newImport.descIdex >= newModule->types->length){
                    printf("%s: Type index of imported function must be defined.\n", newModule->module_name);
                    return -1;
                }*/
                // inc number of func
                *importedFuncCount++;
                break;
            case IMPORT_Table:
                newImport.descType = Desc_Table;
                newImport.descIdex = getLeb128_u32(read_p, end_p);
                break;
            case IMPORT_Mem:
                newImport.descType = Desc_Mem;
                newImport.descIdex = getLeb128_u32(read_p, end_p);
                break;
            case IMPORT_Global:
                newImport.descType = Desc_Global;
                newImport.descIdex = getLeb128_u32(read_p, end_p);
                break;
            default:
                printf("%s: Unknown import type.\n", newModule->module_name);
                return -1;
        }

        // push back into imports vector
        newModule->imports->push_back(newModule->imports, newImport);
    }

    return 0;
}