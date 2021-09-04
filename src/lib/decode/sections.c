/**
 * Copyright 2021 Luis Hsu, Kevin Cyu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include "sections.h"

#include <defines.h>
#include <error.h>
#include <dataTypes/vector_t.h>
#include <Opcodes.h>

#include "utils.h"
#include "parseFuncBody.h"

#define SECTION_PROLOGUE(ID)  \
    if((*read_p < end_p) && (**read_p == ID)) { \
        *read_p += 1; \
        u32_t sectSize = getLeb128_u32(read_p, end_p); \
        const byte_t *start_p = *read_p; \
        if(wasmvm_errno) { \
            return -1; \
        }

#define SECTION_EPILOGUE  \
        i64_t offset = (i64_t)(*read_p - start_p); \
        if(offset != sectSize) { \
            wasmvm_errno = ERROR_sect_size_mis; \
            return -1; \
        } \
    } \
    return 0;

static int read_limits(WasmImport *import, const byte_t **read_p, const byte_t * const end_p)
{
    if(**read_p == 0x01) {
        *read_p += 1;
        import->desc.limits.min = getLeb128_u32(read_p, end_p);
        if(wasmvm_errno) {
            return -1;
        }
        import->desc.limits.max = getLeb128_u32(read_p, end_p);
        if(wasmvm_errno) {
            return -1;
        }
    } else if(**read_p == 0x00) {
        *read_p += 1;
        import->desc.limits.min = getLeb128_u32(read_p, end_p);
        if(wasmvm_errno) {
            return -1;
        }
    } else {
        return -1;
    }
    return 0;
}

static int parse_const_expr(ConstExpr* expr, const byte_t **read_p, const byte_t * const end_p)
{
    switch(*((*read_p)++)) {
        case Op_i32_const:
            expr->type = Const_Value;
            expr->value.type = Value_i32;
            expr->value.value.i32 = (i32_t)getLeb128_i32(read_p, end_p);
            if(wasmvm_errno) {
                return -1;
            }
            break;
        case Op_i64_const:
            expr->type = Const_Value;
            expr->value.type = Value_i64;
            expr->value.value.i64 = (i32_t)getLeb128_i64(read_p, end_p);
            if(wasmvm_errno) {
                return -1;
            }
            break;
        case Op_f32_const:
            expr->type = Const_Value;
            expr->value.type = Value_f32;
            expr->value.value.f32 = toLittle32(*((u32_t*)*read_p), 0);
            if(wasmvm_errno) {
                return -1;
            }
            *read_p += 4;
            break;
        case Op_f64_const:
            expr->type = Const_Value;
            expr->value.type = Value_f64;
            expr->value.value.f64 = toLittle64(*((u64_t*)*read_p), 0);
            if(wasmvm_errno) {
                return -1;
            }
            *read_p += 8;
            break;
        case Op_get_global:
            expr->type = Const_GlobalIndex;
            expr->value.type = Value_i32;
            expr->value.value.u32 = (u32_t)getLeb128_u32(read_p, end_p);
            if(wasmvm_errno) {
                return -1;
            }
            break;
        default:
            wasmvm_errno = ERROR_req_const_expr;
            return -1;
    }
    return 0;
}

int skip_custom_section(const byte_t **read_p, const byte_t * const end_p)
{
    SECTION_PROLOGUE(0)
    *read_p += sectSize;
    // Check
    if(*read_p > end_p) {
        wasmvm_errno = ERROR_int_rep_too_long;
        return -1;
    }
    SECTION_EPILOGUE
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
    SECTION_PROLOGUE(1)
    // Allocate memory
    u32_t typeNum = getLeb128_u32(read_p, end_p);
    if(wasmvm_errno) {
        return -1;
    }
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
        if(wasmvm_errno) {
            return -1;
        }
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
        if(wasmvm_errno) {
            return -1;
        }
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
    SECTION_EPILOGUE
}

int parse_import_section(WasmModule *module, const byte_t **read_p, const byte_t *end_p)
{
    SECTION_PROLOGUE(2)
    // Allocate memory
    u32_t importNum = getLeb128_u32(read_p, end_p);
    if(wasmvm_errno) {
        return -1;
    }
    module->imports.data = (WasmImport*)malloc_func(sizeof(WasmImport) * importNum);
    module->imports.size = importNum;
    // Get all imports
    for(u32_t index = 0; index < importNum; ++index) {
        if(*read_p < end_p) {
            WasmImport* import = module->imports.data + index;
            // Get Module Name length
            u32_t modNameLen = getLeb128_u32(read_p, end_p);
            if(wasmvm_errno) {
                return -1;
            }
            // Get Module Name
            import->module = (char*) malloc_func(sizeof(char) * (modNameLen + 1));
            import->module[modNameLen] = '\0';
            memcpy_func(&(import->module), (const char*)*read_p, modNameLen);
            *read_p += modNameLen;
            // Get Name length
            u32_t nameLen = getLeb128_u32(read_p, end_p);
            if(wasmvm_errno) {
                return -1;
            }
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
                    if(wasmvm_errno) {
                        return -1;
                    }
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
                    wasmvm_errno = ERROR_malform_import;
                    return -1;
            }
        } else {
            wasmvm_errno = ERROR_sect_size_mis;
            return -1;
        }
    }
    SECTION_EPILOGUE
}

int parse_func_section(WasmModule *module, const byte_t **read_p, const byte_t *end_p)
{
    SECTION_PROLOGUE(3)
    // Allocate memory
    u32_t funcNum = getLeb128_u32(read_p, end_p);
    if(wasmvm_errno) {
        return -1;
    }
    module->funcs.data = (WasmFunc*)malloc_func(sizeof(WasmFunc) * funcNum);
    module->funcs.size = funcNum;
    // Get all funcs
    for(u32_t index = 0; index < funcNum; ++index) {
        module->funcs.data[index].type = getLeb128_u32(read_p, end_p);
        if(wasmvm_errno) {
            return -1;
        }
    }
    SECTION_EPILOGUE
}

int parse_table_section(WasmModule *module, const byte_t **read_p, const byte_t *end_p)
{
    SECTION_PROLOGUE(4)
    // Allocate memory
    u32_t tableNum = getLeb128_u32(read_p, end_p);
    if(wasmvm_errno) {
        return -1;
    }
    module->tables.data = (WasmTable*)malloc_func(sizeof(WasmTable) * tableNum);
    module->tables.size = tableNum;
    // Get all tables
    for(u32_t index = 0; index < tableNum; ++index) {
        // Only one table supported
        if(tableNum > 1) {
            wasmvm_errno = ERROR_multi_table;
            return -1;
        }

        // Create WasmTable instance
        byte_t refCode = *((*read_p)++);
        if(refCode == REF_funcref) {
            module->tables.data[index].refType = Ref_func;
        } else if(refCode == REF_externref) {
            module->tables.data[index].refType = Ref_extern;
        } else {
            wasmvm_errno = ERROR_unknown_table;
            return -1;
        }
        byte_t flags = *((*read_p)++);
        module->tables.data[index].min = getLeb128_u32(read_p, end_p);
        if(wasmvm_errno) {
            return -1;
        }
        if(flags == 1) {
            module->tables.data[index].max = getLeb128_u32(read_p, end_p);
            if(wasmvm_errno) {
                return -1;
            }
        } else {
            module->tables.data[index].max = 0;
        }
    }
    SECTION_EPILOGUE
}

int parse_memory_section(WasmModule *module, const byte_t **read_p, const byte_t *end_p)
{
    SECTION_PROLOGUE(5)
    // Allocate memory
    u32_t memNum = getLeb128_u32(read_p, end_p);
    if(wasmvm_errno) {
        return -1;
    }
    module->mems.data = (WasmMemory*)malloc_func(sizeof(WasmMemory) * memNum);
    module->tables.size = memNum;
    // Get all memories
    for(u32_t index = 0; index < memNum; ++index) {
        // Only one memory supported
        if(memNum > 1) {
            wasmvm_errno = ERROR_multi_mem;
            return -1;
        }

        // Create WasmMemory instance
        byte_t flags = *((*read_p)++);
        module->mems.data[index].min = getLeb128_u32(read_p, end_p);
        if(wasmvm_errno) {
            return -1;
        }
        if(flags == 1) {
            module->mems.data[index].max = getLeb128_u32(read_p, end_p);
            if(wasmvm_errno) {
                return -1;
            }
        } else {
            module->mems.data[index].max = 0;
        }
    }
    SECTION_EPILOGUE
}

int parse_global_section(WasmModule *module, const byte_t **read_p, const byte_t *end_p)
{
    SECTION_PROLOGUE(6)
    // Allocate memory
    u32_t globalNum = getLeb128_u32(read_p, end_p);
    if(wasmvm_errno) {
        return -1;
    }
    module->globals.data = (WasmGlobal*)malloc_func(sizeof(WasmGlobal) * globalNum);
    module->globals.size = globalNum;
    // Get all globals
    for(u32_t index = 0; index < globalNum; ++index) {
        WasmGlobal* newGlobal = module->globals.data + index;
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
            case REF_funcref:
                newGlobal->valType = Value_funcref;
                break;
            case REF_externref:
                newGlobal->valType = Value_externref;
                break;
            default:
                wasmvm_errno = ERROR_unknown_global;
                return -1;
        }
        // Set mut
        byte_t mut = *((*read_p)++);
        if((mut != 0x00) && (mut != 0x01)) {
            wasmvm_errno = ERROR_malform_mut;
            return -1;
        }
        newGlobal->mut = mut;
        // Set value
        if(parse_const_expr(&(newGlobal->init), read_p, end_p)) {
            return -1;
        }
        // Skip end
        if(*((*read_p)++) != Op_end) {
            wasmvm_errno = ERROR_unknown_global;
        }
    }
    SECTION_EPILOGUE
}

int parse_export_section(WasmModule *module, const byte_t **read_p, const byte_t *end_p)
{
    SECTION_PROLOGUE(7)
    // Allocate memory
    u32_t exportNum = getLeb128_u32(read_p, end_p);
    if(wasmvm_errno) {
        return -1;
    }
    module->exports.data = (WasmExport*)malloc_func(sizeof(WasmExport) * exportNum);
    module->exports.size = exportNum;
    for(u32_t index = 0; index < exportNum; ++index) {
        WasmExport* newExport = module->exports.data + index;
        // Get name length
        u32_t nameLen = getLeb128_u32(read_p, end_p);
        if(wasmvm_errno) {
            return -1;
        }
        // Get name
        char* name = (char*) malloc(sizeof(char) * (nameLen + 1));
        name[nameLen] = '\0';
        memcpy_func(&name, (const char*)*read_p, nameLen);
        *read_p += nameLen;

        // Export type
        newExport->descType = Desc_Unspecified;
        switch(*((*read_p)++)) {
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
                wasmvm_errno = ERROR_type_mis;
                return -1;
        }
        // Export index
        newExport->descIdx = getLeb128_u32(read_p, end_p);
        if(wasmvm_errno) {
            return -1;
        }
    }
    SECTION_EPILOGUE
}

int parse_start_section(WasmModule *module, const byte_t **read_p, const byte_t *end_p)
{
    SECTION_PROLOGUE(8)
    module->start = getLeb128_u32(read_p, end_p);
    if(wasmvm_errno) {
        return -1;
    }
    SECTION_EPILOGUE
}

int parse_element_section(WasmModule *module, const byte_t **read_p, const byte_t *end_p)
{
    SECTION_PROLOGUE(9)
    // Allocate memory
    u32_t elemNum = getLeb128_u32(read_p, end_p);
    if(wasmvm_errno) {
        return -1;
    }
    module->elems.data = (WasmElem*)malloc_func(sizeof(WasmElem) * elemNum);
    module->elems.size = elemNum;
    // Get all elems
    for(u32_t index = 0; index < elemNum; ++index) {
        WasmElem *newElem = module->elems.data + index;
        // Initial byte
        byte_t initByte = *((*read_p)++);
        // Mode
        if(initByte & 0x01) {
            if(initByte & 0x02) {
                newElem->mode.type = Elem_declarative;
            } else {
                newElem->mode.type = Elem_passive;
            }
        } else {
            newElem->mode.type = Elem_active;
            if(initByte & 0x02) {
                // Explicit table index
                newElem->mode.tableidx = getLeb128_u32(read_p, end_p);
                if(wasmvm_errno) {
                    return -1;
                }
            } else {
                newElem->type = Value_funcref;
            }
            // offset
            if(parse_const_expr(&(newElem->mode.offset), read_p, end_p)) {
                return -1;
            }
        }
        // Init
        if(initByte & 0x04) {
            // Funcidx
            if(initByte & 0x03) {
                if(*((*read_p)++) != 0x00) {
                    // Elemkind should be 0x00 now
                    wasmvm_errno = ERROR_undefined_elem;
                    return -1;
                }
                newElem->type = Value_funcref;
            }
            // Allocate vector
            u32_t idxNum = getLeb128_u32(read_p, end_p);
            if(wasmvm_errno) {
                return -1;
            }
            newElem->init.data = (ConstExpr*)malloc_func(sizeof(ConstExpr) * idxNum);
            newElem->init.size = idxNum;
            // Get funcIdx
            for(u32_t idx = 0; idx < idxNum; ++idx) {
                newElem->init.data[idx].type = Const_Value;
                newElem->init.data[idx].value.type = Value_i32;
                newElem->init.data[idx].value.value.u32 = getLeb128_u32(read_p, end_p);
                if(wasmvm_errno) {
                    return -1;
                }
            }
        } else {
            // Expr
            if(initByte & 0x03) {
                switch (*((*read_p)++)) {
                    case REF_funcref:
                        newElem->type = Value_funcref;
                        break;
                    case REF_externref:
                        newElem->type = Value_externref;
                        break;
                    default:
                        wasmvm_errno = ERROR_undefined_elem;
                        return -1;
                }
            }
            // Allocate vector
            u32_t idxNum = getLeb128_u32(read_p, end_p);
            if(wasmvm_errno) {
                return -1;
            }
            newElem->init.data = (ConstExpr*)malloc_func(sizeof(ConstExpr) * idxNum);
            newElem->init.size = idxNum;
            // Get expr
            for(u32_t idx = 0; idx < idxNum; ++idx) {
                if(parse_const_expr(newElem->init.data + idx, read_p, end_p)) {
                    return -1;
                }
            }
        }
    }
    SECTION_EPILOGUE
}

int parse_code_section(WasmModule *module, const byte_t **read_p, const byte_t *end_p)
{
    SECTION_PROLOGUE(9)
    // Allocate memory
    u32_t codeNum = getLeb128_u32(read_p, end_p);
    if(wasmvm_errno) {
        return -1;
    }
    // Check if code size match func size
    if(codeNum != module->funcs.size) {
        wasmvm_errno = ERROR_inconsist_func;
        return -1;
    }
    // Parse all codes
    for(u32_t index = 0; index < codeNum; ++index) {
        // code size
        u32_t codeSize = getLeb128_u32(read_p, end_p);
        if(wasmvm_errno) {
            return -1;
        }
        const byte_t* bodyEnd = *read_p + codeSize;
        WasmFunc *func = module->funcs.data + index;
        func->locals.size = 0;
        // Locals
        u32_t localSize = getLeb128_u32(read_p, end_p);
        if(wasmvm_errno) {
            return -1;
        }
        for(u32_t localIdx = 0; localIdx < localSize; ++localIdx) {
            // Type count
            u32_t typeCount = getLeb128_u32(read_p, end_p);
            if(wasmvm_errno) {
                return -1;
            }
            ValueType type;
            switch(*((*read_p)++)) {
                case TYPE_i32:
                    type = Value_i32;
                    break;
                case TYPE_i64:
                    type = Value_i64;
                    break;
                case TYPE_f32:
                    type = Value_f32;
                    break;
                case TYPE_f64:
                    type = Value_f64;
                    break;
                default:
                    wasmvm_errno = ERROR_unknown_type;
                    return -1;
            }
            for(u32_t typeIdx = 0; typeIdx < typeCount; ++typeIdx) {
                func->locals.data[func->locals.size + typeIdx] = type;
            }
            func->locals.size += typeCount;
        }
        // Body
        if(parseFuncBody(func, read_p, bodyEnd)) {
            return -1;
        }
    }
    SECTION_EPILOGUE
}

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
