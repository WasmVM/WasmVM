#include "sections.h"
#include <Opcodes.h>

#include "Utils.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

static char skip_to_section(uint8_t sectionNum, uint8_t **ptr, uint8_t **endAddr)
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
        getLeb128_u32(++(ptr), endAddr);
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
        return -2;
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
        // use default free function
        newType->params = new_vector(sizeof(ValueType), NULL);
        newType->results = new_vector(sizeof(ValueType), NULL);
        if(**read_p != TYPE_Func) {
            printf("%s : Function type must start with function type code. (Wrong wasm)\n", newModule->module_name);
            return -1;
        }
        // Params
        uint32_t paramNum = getLeb128_u32(++read_p, end_p);

        while(paramNum-- > 0) {
            ValueType* valType = (ValueType*)malloc(sizeof(ValueType));
            switch(**read_p) {
                case TYPE_i32:
                    *valType = Value_i32;
                    newType->params->push_back(newType->params, (void*)valType);
                    break;
                case TYPE_i64:
                    *valType = Value_i64;
                    newType->params->push_back(newType->params, (void*)valType);
                    break;
                case TYPE_f32:
                    *valType = Value_f32;
                    newType->params->push_back(newType->params, (void*)valType);
                    break;
                case TYPE_f64:
                    *valType = Value_f64;
                    newType->params->push_back(newType->params, (void*)valType);
                    break;
                default:
                    printf("%s : No such parameter type.\n", newModule->module_name);
                    return -1;
            }
            *read_p += 1;
        }

        // Results
        uint32_t resultNum = getLeb128_u32(read_p, end_p);
        while (resultNum-- > 0) {
            ValueType* valType = (ValueType*)malloc(sizeof(ValueType));
            switch (**read_p) {
                case TYPE_i32:
                    *valType = Value_i32;
                    newType->results->push_back(newType->results, (void*)valType);
                    break;
                case TYPE_i64:
                    *valType = Value_i32;
                    newType->results->push_back(newType->results, (void*)valType);
                    break;
                case TYPE_f32:
                    *valType = Value_i32;
                    newType->results->push_back(newType->results, (void*)valType);
                    break;
                case TYPE_f64:
                    *valType = Value_i32;
                    newType->results->push_back(newType->results, (void*)valType);
                    break;
                default:
                    printf("%s : No such parameter type.\n", newModule->module_name);
                    return -1;
            }
            *read_p += 1;
        }
        // Push into newModule
        newModule->types->push_back(newModule->types, (void*)newType);
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
        strncpy(depName, (char*)*read_p, depNameLen);
        *read_p += depNameLen;
        newImport->module = depName;
        // Get Name length
        uint32_t nameLen = getLeb128_u32(read_p, end_p);
        // Get name
        char name[nameLen + 1];
        name[nameLen] = '\0';
        strncpy(name, (char*)*read_p, nameLen);
        *read_p += nameLen;
        newImport->name = name;
        // Load dependencies
        // FIXME: https://github.com/WasmVM/WasmVM/blob/Tiny-Syscall/src/Component/Loader.cpp#L165
        // Does it need to load the dependencies right now?
        // Or just pass the handler to Loader?

        // import kind
        // Reference:
        // - https://webassembly.github.io/spec/core/syntax/modules.html#syntax-index
        // - https://webassembly.github.io/spec/core/binary/modules.html#binary-importsec
        switch(*(*read_p++)) {
            case IMPORT_Func:
                newImport->descType = Desc_Func;
                newImport->desc.typeidx = getLeb128_u32(read_p, end_p);
                // if exceed, then return with error
                /*if(newImport.descIdex >= newModule->types->length){
                    printf("%s: Type index of imported function must be defined.\n", newModule->module_name);
                    return -1;
                }*/
                // inc number of func
                *importedFuncCount += 1;
                break;
            case IMPORT_Table:
                newImport->descType = Desc_Table;
                newImport->desc.typeidx = getLeb128_u32(read_p, end_p);
                break;
            case IMPORT_Mem:
                newImport->descType = Desc_Mem;
                newImport->desc.typeidx = getLeb128_u32(read_p, end_p);
                break;
            case IMPORT_Global:
                newImport->descType = Desc_Global;
                newImport->desc.typeidx = getLeb128_u32(read_p, end_p);
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

int parse_func_section(WasmModule *newModule, uint8_t **read_p, uint8_t **end_p)
{
    uint32_t funcNum = getLeb128_u32(read_p, end_p);
    while(funcNum-- > 0) {
        // allocate WasmFunc
        WasmFunc *newFunc = new_WasmFunc();
        newFunc->type = getLeb128_u32(read_p, end_p);
        // Push into newModule
        newModule->funcs->push_back(newModule->funcs, newFunc);
    }
    return 0;
}

int parse_table_section(WasmModule *newModule, uint8_t **read_p, uint8_t **end_p)
{
    /* FIXME: check current spec */
    if(*((*read_p)++) > 1) {
        printf("%s: There's only one table allowed currently.\n", newModule->module_name);
        return -1;
    }
    if(*((*read_p)++) != TYPE_Table_anyfunc) {
        printf("%s: Only anyfunc is allowed in table currently.\n", newModule->module_name);
        return -1;
    }

    // create WasmTable instance
    WasmTable *newTable = (WasmTable*)malloc(sizeof(WasmTable));
    newTable->elemType = *((*read_p)++);
    newTable->min = getLeb128_u32(read_p, end_p);
    if(newTable->elemType) {
        newTable->max = getLeb128_u32(read_p, end_p);
    }

    // Push into newModule
    newModule->tables->push_back(newModule->tables, newTable);

    return 0;
}

int parse_memory_section(WasmModule *newModule, uint8_t **read_p, uint8_t **end_p)
{
    /* FIXME: check current spec */
    if(*((*read_p)++) > 1) {
        printf("%s: There's only one memory allowed currently.\n", newModule->module_name);
        return -1;
    }

    WasmMemory *newMem = (WasmMemory*)malloc(sizeof(WasmMemory));
    newMem->min = getLeb128_u32(read_p, end_p);
    newMem->max = getLeb128_u32(read_p, end_p);

    newModule->mems->push_back(newModule->mems, newMem);

    return 0;
}

int parse_global_section(WasmModule *newModule, uint8_t **read_p, uint8_t **end_p)
{
    uint32_t globalNum = getLeb128_u32(read_p, end_p);
    while(globalNum-- > 0) {
        WasmGlobal *newGlobal = (WasmGlobal*)malloc(sizeof(WasmGlobal));
        switch(*(*read_p++)) {
            case TYPE_i32:
                newGlobal->valType = Value_i32;
                break;
            case TYPE_i64:
                newGlobal->valType = Value_i64;
                break;
            case TYPE_f32:
                newGlobal->valType = Value_f32;
                break;
            case TYPE_f64:
                newGlobal->valType = Value_f64;
                break;
            default:
                printf("%s: Unknown global value type.\n", newModule->module_name);
                return -1;
        }
        // Set mut
        newGlobal->mut = *(*read_p++);
        // Set value
        switch(*(*read_p++)) {
            case Op_i32_const:
                newGlobal->init.value.i32 = (int32_t)getLeb128_i32(read_p, end_p);
                break;
            case Op_i64_const:
                newGlobal->init.value.i64 = (int64_t)getLeb128_i64(read_p, end_p);
                break;
            case Op_f32_const:
                newGlobal->init.value.f32 = toLittle32(*((uint32_t*)*read_p), 0);
                *read_p += 4;
                break;
            case Op_f64_const:
                newGlobal->init.value.f64 = toLittle64(*((uint64_t*)*read_p), 0);
                *read_p += 8;
                break;
            default:
                printf("%s: Global must be initialized with a constant expression.\n", newModule->module_name);
                return -1;
        }
        // Skip end
        *read_p += 1;
        // Push into newModule
        newModule->globals->push_back(newModule->globals,newGlobal);
    }
    return 0;
}

int parse_export_section(WasmModule *newModule, uint8_t **read_p, uint8_t **end_p)
{
    uint32_t exportNum = getLeb128_u32(read_p, end_p);
    while(exportNum-- > 0) {
        WasmExport* newExport = (WasmExport*)malloc(sizeof(WasmExport));
        // Get name length
        uint32_t nameLen = getLeb128_u32(read_p, end_p);
        // Get name
        char name[nameLen + 1];
        name[nameLen] = '\0';
        strncpy(name, (char*)*read_p, nameLen);
        *read_p += nameLen;
        newExport->name = name;

        // Export kind
        switch(*(*read_p++)) {
            case IMPORT_Func:
                newExport->descType = Desc_Func;
                break;
            case IMPORT_Table:
                newExport->descType = Desc_Table;
                break;
            case IMPORT_Mem:
                newExport->descType = Desc_Mem;
                break;
            case IMPORT_Global:
                newExport->descType = Desc_Global;
                break;
            default:
                printf("%s: Unknown export type.\n", newModule->module_name);
                return -1;
        }
        newExport->descIdx = getLeb128_u32(read_p, end_p);
        // Push into export list
        newModule->exports->push_back(newModule->exports, newExport);
    }

    return 0;
}

int parse_start_section(WasmModule *newModule, uint8_t **read_p, uint8_t **end_p)
{
    newModule->start = getLeb128_u32(read_p, end_p);

    return 0;
}

int parse_element_section(WasmModule *newModule, uint8_t **read_p, uint8_t **end_p)
{
    uint32_t elemNum = getLeb128_u32(read_p, end_p);
    while(elemNum-- > 0) {
        WasmElem *newElem = new_WasmElem();
        // Index
        newElem->table = getLeb128_u32(read_p, end_p);
        if(newElem->table) {
            printf("%s: Only table 0 is allowed currently.\n", newModule->module_name);
            return -1;
        }
        // Offset
        if(*(*read_p++) == Op_i32_const) {
            newElem->offset.value.i32 = getLeb128_i32(read_p, end_p);
        } else {
            printf("%s: Element offset must be an i32.const expression.\n", newModule->module_name);
            return -1;
        }
        // Skip end
        *read_p += 1;
        // Init
        uint32_t initNum = getLeb128_u32(read_p, end_p);
        while(initNum-- > 0) {
            uint32_t initIndex = getLeb128_u32(read_p, end_p);
            if(initIndex >= newModule->funcs->length) {
                printf("%s: Index of element function must be defined.\n", newModule->module_name);
                return -1;
            }
            newElem->init->push_back(newElem->init, &initIndex);
        }
        // Push into newModule
        newModule->elems->push_back(newModule->elems, newElem);
    }

    return 0;
}

int parse_code_section(WasmModule *newModule, uint8_t **read_p, uint8_t **end_p)
{
    uint32_t codeNum = getLeb128_u32(read_p, end_p);
    if(codeNum != newModule->funcs->length) {
        printf("%s: Code count doesn't match function count.\n", newModule->module_name);
        return -1;
    }

    for(uint32_t i = 0; i < codeNum; ++i) {
        uint32_t codeSize = getLeb128_u32(read_p, end_p);
        uint8_t *curAddr = *read_p;
        // Locals
        uint32_t localCount = getLeb128_u32(read_p, end_p);
        while(localCount-- > 0) {
            uint32_t typeCount = getLeb128_u32(read_p, end_p);
            ValueType newType;
            switch(*(*read_p++)) {
                case TYPE_i32:
                    newType = Value_i32;
                    break;
                case TYPE_i64:
                    newType = Value_i64;
                    break;
                case TYPE_f32:
                    newType = Value_f32;
                    break;
                case TYPE_f64:
                    newType = Value_f64;
                    break;
                default:
                    printf("%s: Unknown local type.\n", newModule->module_name);
                    return -1;
            }
            while(typeCount-- > 0) {
                WasmFunc *existedFunc = (WasmFunc*)newModule->funcs->at(newModule->funcs, i);
                existedFunc->locals->push_back(existedFunc->locals, &newType);
            }
        }
        // Code
        codeSize -= (*read_p - curAddr);
        WasmInstr *existedInstr = (WasmInstr*)newModule->funcs->at(newModule->funcs, i);
        existedInstr->opcode = **read_p;
        // shift
        *read_p += codeSize;
    }

    return 0;
}

int parse_data_section(WasmModule *newModule, uint8_t **read_p, uint8_t **end_p)
{
    uint32_t dataNum = getLeb128_u32(read_p, end_p);
    while(dataNum-- > 0) {
        WasmData* newWasmData = new_WasmData();
        // Index
        newWasmData->data = getLeb128_u32(read_p, end_p);
        if(newWasmData->data) {
            printf("%s: Only table 0 is allowed currently.\n", newModule->module_name);
            return -1;
        }
        // Offset
        if(*(*read_p++) == Op_i32_const) {
            newWasmData->offset.value.i32 = getLeb128_i32(read_p, end_p);
        } else {
            printf("%s: Data offset must be an i32.const expression.\n", newModule->module_name);
            return -1;
        }
        *read_p += 1;
        // Init data
        uint32_t dataSize = getLeb128_u32(read_p, end_p);
        char initData[dataSize];
        strncpy(initData, (char*)*read_p, dataSize);
        for(int i=0; i<dataSize; i++) {
            newWasmData->init->push_back(newWasmData->init, initData + i);
        }
        *read_p += dataSize;
        // Push into newModule
        newModule->datas->push_back(newModule->datas, newWasmData);
    }

    return 0;
}
