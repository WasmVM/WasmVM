#include "sections.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include <Opcodes.h>

#include "Utils.h"
#include "parseInstr.h"

static char skip_to_section(uint8_t sectionNum, uint8_t **ptr, const uint8_t *endAddr)
{
    if(*ptr > endAddr) {
        return -1;
    }
    // Check section code, and move ptr to the target
    while(**ptr < sectionNum && **ptr != 0x00) {
        (*ptr)++;
        // Get section size
        uint32_t size = getLeb128_u32(ptr, endAddr);
        *ptr += size;
    }
    char ret = **ptr;
    if(ret == sectionNum) {
        (*ptr)++;
        getLeb128_u32(ptr, endAddr);
    }

    return ret;
}

static int read_limits(WasmImport *import, uint8_t **ptr, const uint8_t *endAddr)
{
    if(**ptr == 0x01) {
        *ptr += 1;
        import->desc.limits.min = getLeb128_u32(ptr, endAddr);
        import->desc.limits.max = getLeb128_u32(ptr, endAddr);
    } else if(**ptr == 0x00) {
        *ptr += 1;
        import->desc.limits.min = getLeb128_u32(ptr, endAddr);
    } else {
        return -1;
    }
    return 0;
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

int parse_type_section(WasmModule *newModule, uint8_t **read_p, const uint8_t *end_p)
{
    if(skip_to_section(1, read_p, end_p) == 1) {
        uint32_t typeNum = getLeb128_u32(read_p, end_p);
        // Get all types
        while(typeNum-- > 0) {
            // FuncType init
            FuncType newType = new_FuncType();
            if(*((*read_p)++) != TYPE_Func) {
                printf("%s : Function type must start with function type code. (Wrong wasm)\n", newModule->module_name);
                return -1;
            }
            // Params
            uint32_t paramNum = getLeb128_u32(read_p, end_p);
            for(uint32_t i = 0; i < paramNum; ++i, ++(*read_p)) {
                ValueType* valType = (ValueType*)malloc(sizeof(ValueType));
                switch(**read_p) {
                    case TYPE_i32:
                        *valType = Value_i32;
                        vector_push_back(newType->params, valType);
                        break;
                    case TYPE_i64:
                        *valType = Value_i64;
                        vector_push_back(newType->params, valType);
                        break;
                    case TYPE_f32:
                        *valType = Value_f32;
                        vector_push_back(newType->params, valType);
                        break;
                    case TYPE_f64:
                        *valType = Value_f64;
                        vector_push_back(newType->params, valType);
                        break;
                    default:
                        printf("%s : No such parameter type.\n", newModule->module_name);
                        return -2;
                }
            }

            // Results
            uint32_t resultNum = getLeb128_u32(read_p, end_p);
            for(uint32_t i = 0; i < resultNum; ++i, ++(*read_p)) {
                ValueType* valType = (ValueType*)malloc(sizeof(ValueType));
                switch (**read_p) {
                    case TYPE_i32:
                        *valType = Value_i32;
                        vector_push_back(newType->results, valType);
                        break;
                    case TYPE_i64:
                        *valType = Value_i32;
                        vector_push_back(newType->results, valType);
                        break;
                    case TYPE_f32:
                        *valType = Value_i32;
                        vector_push_back(newType->results, valType);
                        break;
                    case TYPE_f64:
                        *valType = Value_i32;
                        vector_push_back(newType->results, valType);
                        break;
                    default:
                        printf("%s : No such return type.\n", newModule->module_name);
                        return -3;
                }
            }
            // Push into newModule
            vector_push_back(newModule->types, newType);
        }
    }
    return 0;
}

int parse_import_section(WasmModule *newModule, uint8_t **read_p, const uint8_t *end_p, Loader loader, Executor executor)
{
    if(skip_to_section(2, read_p, end_p) == 2) {
        for(uint32_t importNum = getLeb128_u32(read_p, end_p); importNum > 0; --importNum) {
            WasmImport *newImport = (WasmImport*)malloc(sizeof(WasmImport));
            // Get Module Name length
            uint32_t depNameLen = getLeb128_u32(read_p, end_p);
            // Get Module Name
            char* depName = (char*) malloc(sizeof(char) * (depNameLen + 1));
            depName[depNameLen] = '\0';
            strncpy(depName, (char*)*read_p, depNameLen);
            *read_p += depNameLen;
            newImport->module = depName;
            // Get Name length
            uint32_t nameLen = getLeb128_u32(read_p, end_p);
            // Get name
            char* name = (char*) malloc(sizeof(char) * (nameLen + 1));
            name[nameLen] = '\0';
            strncpy(name, (char*)*read_p, nameLen);
            *read_p += nameLen;
            newImport->name = name;
            // Load dependencies
            loader_addRequest(loader, new_LoaderRequest(newImport->module, (Component*)loader, executor));
            // import kind
            switch(*((*read_p)++)) {
                case IMPORT_Func:
                    newImport->descType = Desc_Func;
                    newImport->desc.typeidx = getLeb128_u32(read_p, end_p);
                    break;
                case IMPORT_Table:
                    if(*((*read_p)++) != TYPE_Table_anyfunc) {
                        return -2;
                    }
                    newImport->descType = Desc_Table;
                    if(read_limits(newImport, read_p, end_p)) {
                        return -3;
                    }
                    break;
                case IMPORT_Mem:
                    newImport->descType = Desc_Mem;
                    if(read_limits(newImport, read_p, end_p)) {
                        return -4;
                    }
                    break;
                case IMPORT_Global:
                    newImport->descType = Desc_Global;
                    switch (*((*read_p)++)) {
                        case TYPE_i32:
                            newImport->desc.global.valueType = Value_i32;
                            break;
                        case TYPE_i64:
                            newImport->desc.global.valueType = Value_i64;
                            break;
                        case TYPE_f32:
                            newImport->desc.global.valueType = Value_f32;
                            break;
                        case TYPE_f64:
                            newImport->desc.global.valueType = Value_f64;
                            break;
                        default:
                            return -5;
                    }
                    if((**read_p == 0x01) || (**read_p == 0x00)) {
                        newImport->desc.global.mut = **read_p;
                    } else {
                        return -6;
                    }
                    break;
                default:
                    printf("%s: Unknown import type.\n", newModule->module_name);
                    return -1;
            }

            // push back into imports vector
            vector_push_back(newModule->imports, newImport);
        }
    }
    return 0;
}

int parse_func_section(WasmModule *newModule, uint8_t **read_p, const uint8_t *end_p)
{
    if(skip_to_section(3, read_p, end_p) == 3) {
        for(uint32_t funcNum = getLeb128_u32(read_p, end_p); funcNum > 0; --funcNum) {
            // allocate WasmFunc
            WasmFunc *newFunc = new_WasmFunc();
            newFunc->type = getLeb128_u32(read_p, end_p);
            // Push into newModule
            vector_push_back(newModule->funcs, newFunc);
        }
    }
    return 0;
}

int parse_table_section(WasmModule *newModule, uint8_t **read_p, const uint8_t *end_p)
{
    if(skip_to_section(4, read_p, end_p) == 4) {
        for(uint32_t tableNum = getLeb128_u32(read_p, end_p); tableNum > 0; --tableNum) {
            if(tableNum > 1) {
                printf("%s: There's only one table allowed currently.\n", newModule->module_name);
                return -1;
            }

            // create WasmTable instance
            WasmTable *newTable = (WasmTable*)malloc(sizeof(WasmTable));
            newTable->elemType = *((*read_p)++);
            if(newTable->elemType != TYPE_Table_anyfunc) {
                printf("%s: Only anyfunc is allowed in table currently.\n", newModule->module_name);
                return -2;
            }
            uint8_t flags = *((*read_p)++);
            newTable->min = getLeb128_u32(read_p, end_p);
            if(flags == 1) {
                newTable->max = getLeb128_u32(read_p, end_p);
            }

            // Push into newModule
            vector_push_back(newModule->tables, newTable);
        }
    }
    return 0;
}

int parse_memory_section(WasmModule *newModule, uint8_t **read_p, const uint8_t *end_p)
{
    if(skip_to_section(5, read_p, end_p) == 5) {
        for(uint32_t memNum = getLeb128_u32(read_p, end_p); memNum > 0; --memNum) {
            if(memNum > 1) {
                printf("%s: There's only one memory allowed currently.\n", newModule->module_name);
                return -1;
            }

            WasmMemory *newMem = (WasmMemory*)malloc(sizeof(WasmMemory));
            uint8_t flags = *((*read_p)++);
            newMem->min = getLeb128_u32(read_p, end_p);
            if(flags == 1) {
                newMem->max = getLeb128_u32(read_p, end_p);
            }

            vector_push_back(newModule->mems, newMem);
        }
    }
    return 0;
}

int parse_global_section(WasmModule *newModule, uint8_t **read_p, const uint8_t *end_p)
{
    if(skip_to_section(6, read_p, end_p) == 6) {
        for(uint32_t globalNum = getLeb128_u32(read_p, end_p); globalNum > 0; --globalNum) {
            WasmGlobal *newGlobal = (WasmGlobal*)malloc(sizeof(WasmGlobal));
            switch(*((*read_p)++)) {
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
            newGlobal->mut = *((*read_p)++);
            // Set value
            switch(*((*read_p)++)) {
                case Op_i32_const:
                    newGlobal->init.type = Value_i32;
                    newGlobal->init.value.i32 = (int32_t)getLeb128_i32(read_p, end_p);
                    break;
                case Op_i64_const:
                    newGlobal->init.type = Value_i64;
                    newGlobal->init.value.i64 = (int64_t)getLeb128_i64(read_p, end_p);
                    break;
                case Op_f32_const:
                    newGlobal->init.type = Value_f32;
                    newGlobal->init.value.u32 = toLittle32(*((uint32_t*)*read_p), 0);
                    *read_p += 4;
                    break;
                case Op_f64_const:
                    newGlobal->init.type = Value_f64;
                    newGlobal->init.value.u64 = toLittle64(*((uint64_t*)*read_p), 0);
                    *read_p += 8;
                    break;
                default:
                    printf("%s: Global must be initialized with a constant expression.\n", newModule->module_name);
                    return -2;
            }
            // Skip end
            *read_p += 1;
            // Push into newModule
            vector_push_back(newModule->globals,newGlobal);
        }
    }
    return 0;
}

int parse_export_section(WasmModule *newModule, uint8_t **read_p, const uint8_t *end_p)
{
    if(skip_to_section(7, read_p, end_p) == 7) {
        for(uint32_t exportNum = getLeb128_u32(read_p, end_p); exportNum > 0; --exportNum) {
            // Get name length
            uint32_t nameLen = getLeb128_u32(read_p, end_p);
            // Get name
            char* name = (char*) malloc(sizeof(char) * (nameLen + 1));
            name[nameLen] = '\0';
            strncpy(name, (char*)*read_p, nameLen);
            *read_p += nameLen;

            // Export kind
            DescType descType = Desc_Unspecified;
            switch(*((*read_p)++)) {
                case IMPORT_Func:
                    descType = Desc_Func;
                    break;
                case IMPORT_Table:
                    descType = Desc_Table;
                    break;
                case IMPORT_Mem:
                    descType = Desc_Mem;
                    break;
                case IMPORT_Global:
                    descType = Desc_Global;
                    break;
                default:
                    printf("%s: Unknown export type.\n", newModule->module_name);
                    return -1;
            }
            // Push into export list_p
            vector_push_back(newModule->exports, new_WasmExport(name, descType, getLeb128_u32(read_p, end_p)));
        }
    }
    return 0;
}

int parse_start_section(WasmModule *newModule, uint8_t **read_p, const uint8_t *end_p)
{
    if(skip_to_section(8, read_p, end_p) == 8) {
        newModule->start = getLeb128_u32(read_p, end_p);
    }
    return 0;
}

int parse_element_section(WasmModule *newModule, uint8_t **read_p, const uint8_t *end_p)
{
    if(skip_to_section(9, read_p, end_p) == 9) {
        for(uint32_t elemNum = getLeb128_u32(read_p, end_p); elemNum > 0; --elemNum) {
            WasmElem *newElem = new_WasmElem();
            // Index
            newElem->table = getLeb128_u32(read_p, end_p);
            if(newElem->table) {
                printf("%s: Only table 0 is allowed currently.\n", newModule->module_name);
                return -1;
            }
            // Offset
            if(*((*read_p)++) == Op_i32_const) {
                newElem->offset.type = Value_i32;
                newElem->offset.value.i32 = getLeb128_i32(read_p, end_p);
            } else {
                printf("%s: Element offset must be an i32.const expression.\n", newModule->module_name);
                return -2;
            }
            // Skip end
            *read_p += 1;
            // Init
            for(uint32_t initNum = getLeb128_u32(read_p, end_p); initNum > 0; --initNum) {
                uint32_t initIndex = getLeb128_u32(read_p, end_p);
                vector_push_back(newElem->init, &initIndex);
            }
            // Push into newModule
            vector_push_back(newModule->elems, newElem);
        }
    }
    return 0;
}

int parse_code_section(WasmModule *newModule, uint8_t **read_p, const uint8_t *end_p)
{
    if(skip_to_section(10, read_p, end_p) == 10) {
        uint32_t codeNum = getLeb128_u32(read_p, end_p);
        if(codeNum != vector_size(newModule->funcs)) {
            printf("%s: Code count doesn't match function count.\n", newModule->module_name);
            return -1;
        }
        for(uint32_t i = 0; i < codeNum; ++i) {
            uint8_t* bodyEnd = *read_p + getLeb128_u32(read_p, end_p);
            WasmFunc *func = vector_at(WasmFunc*, newModule->funcs, i);
            // Locals
            for(uint32_t localCount = getLeb128_u32(read_p, end_p); localCount > 0; --localCount) {
                uint32_t typeCount = getLeb128_u32(read_p, end_p);
                ValueType newType;
                switch(*((*read_p)++)) {
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
                        return -2;
                }
                while(typeCount-- > 0) {
                    vector_push_back(func->locals, &newType);
                }
            }
            // Code
            while(*read_p != bodyEnd) {
                if(parseInstr(func, read_p, end_p)) {
                    return -3;
                }
            }
            (*read_p)++;
        }
    }
    return 0;
}

int parse_data_section(WasmModule *newModule, uint8_t **read_p, const uint8_t *end_p)
{
    if(skip_to_section(11, read_p, end_p) == 11) {
        for(uint32_t dataNum = getLeb128_u32(read_p, end_p); dataNum > 0; --dataNum) {
            WasmData* newWasmData = new_WasmData();
            // Index
            newWasmData->data = getLeb128_u32(read_p, end_p);
            if(newWasmData->data) {
                printf("%s: Only table 0 is allowed currently.\n", newModule->module_name);
                return -1;
            }
            // Offset
            if(*((*read_p)++) == Op_i32_const) {
                newWasmData->offset.type = Value_i32;
                newWasmData->offset.value.i32 = getLeb128_i32(read_p, end_p);
            } else {
                printf("%s: Data offset must be an i32.const expression.\n", newModule->module_name);
                return -2;
            }
            *read_p += 1;
            // Init data
            vector_resize(newWasmData->init, getLeb128_u32(read_p, end_p));
            strncpy(vector_data(char*, newWasmData->init), (char*)*read_p, vector_size(newWasmData->init));
            *read_p += vector_size(newWasmData->init);
            // Push into newModule
            vector_push_back(newModule->datas, newWasmData);
        }
    }
    return 0;
}
