/**
 * Copyright 2021 Luis Hsu, Kevin Cyu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include "sections.h"

#include <defines.h>
#include <error.h>
#include <dataTypes/vector_t.h>
// #include <Opcodes.h>

#include "utils.h"
// #include "parseInstr.h"

static int read_limits(WasmImport *import, const byte_t **read_p, const byte_t * const end_p)
{
    if(**read_p == 0x01) {
        *read_p += 1;
        import->desc.limits.min = getLeb128_u32(read_p, end_p);
        import->desc.limits.max = getLeb128_u32(read_p, end_p);
    } else if(**read_p == 0x00) {
        *read_p += 1;
        import->desc.limits.min = getLeb128_u32(read_p, end_p);
    } else {
        return -1;
    }
    return 0;
}

int skip_custom_section(const byte_t **read_p, const byte_t * const end_p)
{
    if((*read_p < end_p) && (**read_p == 0x00)) {
        ++(*read_p);
        *read_p += getLeb128_u32(read_p, end_p);
        // Check
        if(*read_p > end_p) {
            wasmvm_errno = ERROR_int_rep_too_long;
            return -1;
        } else {
            return 0;
        }
    }
    return 0;
}

int parse_magic_version(const byte_t **read_p, const byte_t * const end_p)
{
    // Magic
    if((end_p - *read_p) < sizeof(u32_t)) {
        wasmvm_errno = ERROR_unexpected_end;
        return -1;
    }
    if(*(u32_t*) *read_p != toLittle32(WASM_MAGIC, 0)) {
        wasmvm_errno = ERROR_magic_not_detect;
        return -1;
    }
    *read_p += 4;

    // Version
    if((end_p - *read_p) < sizeof(u32_t)) {
        wasmvm_errno = ERROR_unexpected_end;
        return -1;
    }
    if(*(u32_t*) *read_p != toLittle32(WASM_VERSION, 0)) {
        wasmvm_errno = ERROR_unknown_bin_ver;
        return -1;
    }
    *read_p += 4;

    // Check
    if(*read_p > end_p) {
        wasmvm_errno = ERROR_unexpected_end;
        return -1;
    } else {
        return 0;
    }
}

int parse_type_section(WasmModule *module, const byte_t **read_p, const byte_t * const end_p)
{
    if(**read_p == 1) {
        // Save original pointer
        const byte_t *origin_p = ++(*read_p);
        u32_t sectSize = getLeb128_u32(read_p, end_p);
        // Allocate memory
        u32_t typeNum = getLeb128_u32(read_p, end_p);
        module->types.data = (FuncType*)malloc_func(sizeof(FuncType) * typeNum);
        module->types.size = typeNum;
        // Get all types
        for(u32_t index = 0; index < typeNum; ++index) {
            FuncType* funcType = module->types.data + index;
            // FuncType check
            if(*((*read_p)++) != TYPE_Func) {
                wasmvm_errno = ERROR_int_rep_too_long;
                return -1;
            }
            // Params
            u32_t paramNum = getLeb128_u32(read_p, end_p);
            funcType->params.data = (ValueType*)malloc_func(sizeof(ValueType) * paramNum);
            funcType->params.size = paramNum;
            for(u32_t i = 0; i < paramNum; ++i, ++(*read_p)) {
                switch(**read_p) {
                    case TYPE_i32:
                        funcType->params.data[i] = Value_i32;
                        break;
                    case TYPE_i64:
                        funcType->params.data[i] = Value_i64;
                        break;
                    case TYPE_f32:
                        funcType->params.data[i] = Value_f32;
                        break;
                    case TYPE_f64:
                        funcType->params.data[i] = Value_f64;
                        break;
                    default:
                        wasmvm_errno = ERROR_unknown_type;
                        return -1;
                }
            }
            // Results
            u32_t resultNum = getLeb128_u32(read_p, end_p);
            funcType->results.data = (ValueType*)malloc_func(sizeof(ValueType) * resultNum);
            funcType->results.size = resultNum;
            for(u32_t i = 0; i < resultNum; ++i, ++(*read_p)) {
                switch (**read_p) {
                    case TYPE_i32:
                        funcType->results.data[i] = Value_i32;
                        break;
                    case TYPE_i64:
                        funcType->results.data[i] = Value_i64;
                        break;
                    case TYPE_f32:
                        funcType->results.data[i] = Value_f32;
                        break;
                    case TYPE_f64:
                        funcType->results.data[i] = Value_f64;
                        break;
                    default:
                        wasmvm_errno = ERROR_unknown_type;
                        return -1;
                }
            }
        }
        // Check size
        i64_t offset = (i64_t)(*read_p - origin_p);
        if(offset != (sectSize + 1)) {
            wasmvm_errno = ERROR_sect_size_mis;
            return -1;
        }
    }
    return 0;
}

int parse_import_section(WasmModule *module, const byte_t **read_p, const byte_t *end_p)
{
    if(**read_p == 2) {
        // Save original pointer
        const byte_t *origin_p = ++(*read_p);
        u32_t sectSize = getLeb128_u32(read_p, end_p);
        // Allocate memory
        u32_t importNum = getLeb128_u32(read_p, end_p);
        module->imports.data = (WasmImport*)malloc_func(sizeof(WasmImport) * importNum);
        module->imports.size = importNum;
        // Get all imports
        for(u32_t index = 0; index < importNum; ++index) {
            WasmImport* import = module->imports.data + index;
            // Get Module Name length
            u32_t modNameLen = getLeb128_u32(read_p, end_p);
            // Get Module Name
            import->module = (char*) malloc_func(sizeof(char) * (modNameLen + 1));
            import->module[modNameLen] = '\0';
            memcpy_func(&(import->module), (const char*)*read_p, modNameLen);
            *read_p += modNameLen;
            // Get Name length
            u32_t nameLen = getLeb128_u32(read_p, end_p);
            // Get name
            import->name = (char*) malloc_func(sizeof(char) * (nameLen + 1));
            import->name[nameLen] = '\0';
            memcpy_func(&(import->name), (const char*)*read_p, nameLen);
            *read_p += nameLen;
            // import kind
            switch(*((*read_p)++)) {
                case IMPORT_Func:
                    import->descType = Desc_Func;
                    import->desc.typeidx = getLeb128_u32(read_p, end_p);
                    break;
                case IMPORT_Table:
                    if((*((*read_p)++) != REF_funcref) && (*((*read_p)++) != REF_externref)) {
                        wasmvm_errno = ERROR_malform_import;
                        return -1;
                    }
                    import->descType = Desc_Table;
                    if(read_limits(import, read_p, end_p)) {
                        wasmvm_errno = ERROR_malform_import;
                        return -1;
                    }
                    break;
                case IMPORT_Mem:
                    import->descType = Desc_Mem;
                    if(read_limits(import, read_p, end_p)) {
                        wasmvm_errno = ERROR_malform_import;
                        return -1;
                    }
                    break;
                case IMPORT_Global:
                    import->descType = Desc_Global;
                    switch (*((*read_p)++)) {
                        case TYPE_i32:
                            import->desc.global.valueType = Value_i32;
                            break;
                        case TYPE_i64:
                            import->desc.global.valueType = Value_i64;
                            break;
                        case TYPE_f32:
                            import->desc.global.valueType = Value_f32;
                            break;
                        case TYPE_f64:
                            import->desc.global.valueType = Value_f64;
                            break;
                        default:
                            wasmvm_errno = ERROR_unknown_global;
                            return -1;
                    }
                    if((**read_p == 0x01) || (**read_p == 0x00)) {
                        import->desc.global.mut = **read_p;
                    } else {
                        wasmvm_errno = ERROR_unexpected_token;
                        return -1;
                    }
                    break;
                default:
                    wasmvm_errno = ERROR_incomp_import;
                    return -1;
            }
        }
        // Check size
        i64_t offset = (i64_t)(*read_p - origin_p);
        if(offset != (sectSize + 1)) {
            wasmvm_errno = ERROR_sect_size_mis;
            return -1;
        }
    }
    return 0;
}

// int parse_func_section(WasmModule *newModule, uint8_t **read_p, const uint8_t *end_p)
// {
//     if(skip_to_section(3, read_p, end_p) == 3) {
//         for(uint32_t funcNum = getLeb128_u32(read_p, end_p); funcNum > 0; --funcNum) {
//             // allocate WasmFunc
//             WasmFunc *newFunc = new_WasmFunc();
//             newFunc->type = getLeb128_u32(read_p, end_p);
//             // Push into newModule
//             vector_push_back(newModule->funcs, newFunc);
//         }
//     }
//     return 0;
// }

// int parse_table_section(WasmModule *newModule, uint8_t **read_p, const uint8_t *end_p)
// {
//     if(skip_to_section(4, read_p, end_p) == 4) {
//         for(uint32_t tableNum = getLeb128_u32(read_p, end_p); tableNum > 0; --tableNum) {
//             if(tableNum > 1) {
//                 printf("%s: There's only one table allowed currently.\n", newModule->module_name);
//                 return -1;
//             }

//             // create WasmTable instance
//             WasmTable *newTable = (WasmTable*)malloc(sizeof(WasmTable));
//             newTable->elemType = *((*read_p)++);
//             if(newTable->elemType != TYPE_Table_anyfunc) {
//                 printf("%s: Only anyfunc is allowed in table currently.\n", newModule->module_name);
//                 return -2;
//             }
//             uint8_t flags = *((*read_p)++);
//             newTable->min = getLeb128_u32(read_p, end_p);
//             if(flags == 1) {
//                 newTable->max = getLeb128_u32(read_p, end_p);
//             }

//             // Push into newModule
//             vector_push_back(newModule->tables, newTable);
//         }
//     }
//     return 0;
// }

// int parse_memory_section(WasmModule *newModule, uint8_t **read_p, const uint8_t *end_p)
// {
//     if(skip_to_section(5, read_p, end_p) == 5) {
//         for(uint32_t memNum = getLeb128_u32(read_p, end_p); memNum > 0; --memNum) {
//             if(memNum > 1) {
//                 printf("%s: There's only one memory allowed currently.\n", newModule->module_name);
//                 return -1;
//             }

//             WasmMemory *newMem = (WasmMemory*)malloc(sizeof(WasmMemory));
//             uint8_t flags = *((*read_p)++);
//             newMem->min = getLeb128_u32(read_p, end_p);
//             if(flags == 1) {
//                 newMem->max = getLeb128_u32(read_p, end_p);
//             }

//             vector_push_back(newModule->mems, newMem);
//         }
//     }
//     return 0;
// }

// int parse_global_section(WasmModule *newModule, uint8_t **read_p, const uint8_t *end_p)
// {
//     if(skip_to_section(6, read_p, end_p) == 6) {
//         for(uint32_t globalNum = getLeb128_u32(read_p, end_p); globalNum > 0; --globalNum) {
//             WasmGlobal *newGlobal = (WasmGlobal*)malloc(sizeof(WasmGlobal));
//             switch(*((*read_p)++)) {
//                 case TYPE_i32:
//                     newGlobal->valType = Value_i32;
//                     break;
//                 case TYPE_i64:
//                     newGlobal->valType = Value_i64;
//                     break;
//                 case TYPE_f32:
//                     newGlobal->valType = Value_f32;
//                     break;
//                 case TYPE_f64:
//                     newGlobal->valType = Value_f64;
//                     break;
//                 default:
//                     printf("%s: Unknown global value type.\n", newModule->module_name);
//                     return -1;
//             }
//             // Set mut
//             newGlobal->mut = *((*read_p)++);
//             // Set value
//             switch(*((*read_p)++)) {
//                 case Op_i32_const:
//                     newGlobal->init.type = Value_i32;
//                     newGlobal->init.value.i32 = (int32_t)getLeb128_i32(read_p, end_p);
//                     break;
//                 case Op_i64_const:
//                     newGlobal->init.type = Value_i64;
//                     newGlobal->init.value.i64 = (int64_t)getLeb128_i64(read_p, end_p);
//                     break;
//                 case Op_f32_const:
//                     newGlobal->init.type = Value_f32;
//                     newGlobal->init.value.u32 = toLittle32(*((uint32_t*)*read_p), 0);
//                     *read_p += 4;
//                     break;
//                 case Op_f64_const:
//                     newGlobal->init.type = Value_f64;
//                     newGlobal->init.value.u64 = toLittle64(*((uint64_t*)*read_p), 0);
//                     *read_p += 8;
//                     break;
//                 default:
//                     printf("%s: Global must be initialized with a constant expression.\n", newModule->module_name);
//                     return -2;
//             }
//             // Skip end
//             *read_p += 1;
//             // Push into newModule
//             vector_push_back(newModule->globals,newGlobal);
//         }
//     }
//     return 0;
// }

// int parse_export_section(WasmModule *newModule, uint8_t **read_p, const uint8_t *end_p)
// {
//     if(skip_to_section(7, read_p, end_p) == 7) {
//         for(uint32_t exportNum = getLeb128_u32(read_p, end_p); exportNum > 0; --exportNum) {
//             // Get name length
//             uint32_t nameLen = getLeb128_u32(read_p, end_p);
//             // Get name
//             char* name = (char*) malloc(sizeof(char) * (nameLen + 1));
//             name[nameLen] = '\0';
//             strncpy(name, (char*)*read_p, nameLen);
//             *read_p += nameLen;

//             // Export kind
//             DescType descType = Desc_Unspecified;
//             switch(*((*read_p)++)) {
//                 case IMPORT_Func:
//                     descType = Desc_Func;
//                     break;
//                 case IMPORT_Table:
//                     descType = Desc_Table;
//                     break;
//                 case IMPORT_Mem:
//                     descType = Desc_Mem;
//                     break;
//                 case IMPORT_Global:
//                     descType = Desc_Global;
//                     break;
//                 default:
//                     printf("%s: Unknown export type.\n", newModule->module_name);
//                     return -1;
//             }
//             // Push into export list_p
//             vector_push_back(newModule->exports, new_WasmExport(name, descType, getLeb128_u32(read_p, end_p)));
//         }
//     }
//     return 0;
// }

// int parse_start_section(WasmModule *newModule, uint8_t **read_p, const uint8_t *end_p)
// {
//     if(skip_to_section(8, read_p, end_p) == 8) {
//         newModule->start = getLeb128_u32(read_p, end_p);
//     }
//     return 0;
// }

// int parse_element_section(WasmModule *newModule, uint8_t **read_p, const uint8_t *end_p)
// {
//     if(skip_to_section(9, read_p, end_p) == 9) {
//         for(uint32_t elemNum = getLeb128_u32(read_p, end_p); elemNum > 0; --elemNum) {
//             WasmElem *newElem = new_WasmElem();
//             // Index
//             newElem->table = getLeb128_u32(read_p, end_p);
//             if(newElem->table) {
//                 printf("%s: Only table 0 is allowed currently.\n", newModule->module_name);
//                 return -1;
//             }
//             // Offset
//             if(*((*read_p)++) == Op_i32_const) {
//                 newElem->offset.type = Value_i32;
//                 newElem->offset.value.i32 = getLeb128_i32(read_p, end_p);
//             } else {
//                 printf("%s: Element offset must be an i32.const expression.\n", newModule->module_name);
//                 return -2;
//             }
//             // Skip end
//             *read_p += 1;
//             // Init
//             for(uint32_t initNum = getLeb128_u32(read_p, end_p); initNum > 0; --initNum) {
//                 uint32_t initIndex = getLeb128_u32(read_p, end_p);
//                 vector_push_back(newElem->init, &initIndex);
//             }
//             // Push into newModule
//             vector_push_back(newModule->elems, newElem);
//         }
//     }
//     return 0;
// }

// int parse_code_section(WasmModule *newModule, uint8_t **read_p, const uint8_t *end_p)
// {
//     if(skip_to_section(10, read_p, end_p) == 10) {
//         uint32_t codeNum = getLeb128_u32(read_p, end_p);
//         if(codeNum != vector_size(newModule->funcs)) {
//             printf("%s: Code count doesn't match function count.\n", newModule->module_name);
//             return -1;
//         }
//         for(uint32_t i = 0; i < codeNum; ++i) {
//             uint8_t* bodyEnd = *read_p + getLeb128_u32(read_p, end_p);
//             WasmFunc *func = vector_at(WasmFunc*, newModule->funcs, i);
//             // Locals
//             for(uint32_t localCount = getLeb128_u32(read_p, end_p); localCount > 0; --localCount) {
//                 uint32_t typeCount = getLeb128_u32(read_p, end_p);
//                 ValueType newType;
//                 switch(*((*read_p)++)) {
//                     case TYPE_i32:
//                         newType = Value_i32;
//                         break;
//                     case TYPE_i64:
//                         newType = Value_i64;
//                         break;
//                     case TYPE_f32:
//                         newType = Value_f32;
//                         break;
//                     case TYPE_f64:
//                         newType = Value_f64;
//                         break;
//                     default:
//                         printf("%s: Unknown local type.\n", newModule->module_name);
//                         return -2;
//                 }
//                 while(typeCount-- > 0) {
//                     vector_push_back(func->locals, &newType);
//                 }
//             }
//             // Code
//             while(*read_p != bodyEnd) {
//                 if(parseInstr(func, read_p, end_p)) {
//                     return -3;
//                 }
//             }
//             (*read_p)++;
//         }
//     }
//     return 0;
// }

// int parse_data_section(WasmModule *newModule, uint8_t **read_p, const uint8_t *end_p)
// {
//     if(skip_to_section(11, read_p, end_p) == 11) {
//         for(uint32_t dataNum = getLeb128_u32(read_p, end_p); dataNum > 0; --dataNum) {
//             WasmData* newWasmData = new_WasmData();
//             // Index
//             newWasmData->data = getLeb128_u32(read_p, end_p);
//             if(newWasmData->data) {
//                 printf("%s: Only table 0 is allowed currently.\n", newModule->module_name);
//                 return -1;
//             }
//             // Offset
//             if(*((*read_p)++) == Op_i32_const) {
//                 newWasmData->offset.type = Value_i32;
//                 newWasmData->offset.value.i32 = getLeb128_i32(read_p, end_p);
//             } else {
//                 printf("%s: Data offset must be an i32.const expression.\n", newModule->module_name);
//                 return -2;
//             }
//             *read_p += 1;
//             // Init data
//             vector_resize(newWasmData->init, getLeb128_u32(read_p, end_p));
//             strncpy(vector_data(char*, newWasmData->init), (char*)*read_p, vector_size(newWasmData->init));
//             *read_p += vector_size(newWasmData->init);
//             // Push into newModule
//             vector_push_back(newModule->datas, newWasmData);
//         }
//     }
//     return 0;
// }
