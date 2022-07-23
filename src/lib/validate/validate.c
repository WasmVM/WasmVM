/**
 * Copyright 2022 Luis Hsu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include <WasmVM.h>
#include "validate.h"

_Bool module_validate(wasm_module module){
    // Initialize context
    ValidateContext context;
    vector_init(context.funcs);
    vector_init(context.tables);
    vector_init(context.mems);
    vector_init(context.globals);
    u32_t funcSize = 0;
    u32_t tableSize = 0;
    u32_t memSize = 0;
    u32_t globalSize = 0;
    for(size_t i = 0; i < module->imports.size; ++i){
        WasmImport* import = module->imports.data + i;
        switch (import->descType){
            case Desc_Func:
                funcSize += 1;
                break;
            case Desc_Table:
                tableSize += 1;
                break;
            case Desc_Mem:
                memSize += 1;
                break;
            case Desc_Global:
                globalSize += 1;
                break;
            default:
                wasmvm_errno = ERROR_unknown_import;
                return 0;
        }
    }
    if(funcSize > 0){
        vector_resize(context.funcs, u32_t, funcSize);
    }
    if(tableSize > 0){
        vector_resize(context.tables, WasmTable, tableSize);
    }
    if(memSize > 0){
        vector_resize(context.mems, WasmMemory, memSize);
    }
    if(globalSize > 0){
        vector_resize(context.globals, ValueType, globalSize);
    }
    for(size_t i = 0; i < module->imports.size; ++i){
        WasmImport* import = module->imports.data + i;
        switch (import->descType){
            case Desc_Func:
                context.funcs.data[context.funcs.size] = import->desc.typeidx;
                context.funcs.size += 1;
                break;
            case Desc_Table:
                context.tables.data[context.tables.size].min = import->desc.limits.min;
                context.tables.data[context.tables.size].max = import->desc.limits.max;
                context.tables.data[context.tables.size].refType = import->desc.limits.type;
                context.tables.size += 1;
                break;
            case Desc_Mem:
                context.mems.data[context.mems.size].min = import->desc.limits.min;
                context.mems.data[context.mems.size].max = import->desc.limits.max;
                context.mems.size += 1;
                break;
            case Desc_Global:
                context.globals.data[context.globals.size] = import->desc.global.valueType;
                context.globals.size += 1;
                break;
            default:
                wasmvm_errno = ERROR_unknown_import;
                return 0;
        }
    }
    // Functions
    for(size_t i = 0; i < module->funcs.size; ++i){
        if(!func_validate(module->funcs.data + i, module, &context)){
            return 0;
        }
    }
    // Tables
    for(size_t i = 0; i < module->tables.size; ++i){
        if(!table_validate(module->tables.data + i)){
            wasmvm_errno = ERROR_len_out_of_bound;
            return 0;
        }
    }
    // Mems
    for(size_t i = 0; i < module->mems.size; ++i){
        if(!memory_validate(module->mems.data + i)){
            wasmvm_errno = ERROR_len_out_of_bound;
            return 0;
        }
    }
    // Globals
    for(size_t i = 0; i < module->globals.size; ++i){
        if(!global_validate(module->globals.data + i, module, &context)){
            return 0;
        }
    }
    // Elems
    for(size_t i = 0; i < module->elems.size; ++i){
        if(!elem_validate(module->elems.data + i, module, &context)){
            return 0;
        }
    }
    return 1;
}

_Bool table_validate(WasmTable* table){
    return (!table->max) || (table->min <= table->max);
}

_Bool memory_validate(WasmMemory* memory){
    return (!memory->max) || (memory->min <= memory->max);
}

_Bool global_validate(WasmGlobal* global, WasmModule* module, ValidateContext* context){
    switch(global->init.type){
        case Const_GlobalIndex:{
            u32_t index = global->init.value.value.u32;
            if(index >= (module->globals.size + context->globals.size)){
                wasmvm_errno = ERROR_len_out_of_bound;
                return 0;
            }
            if(index >= context->globals.size){
                if(module->globals.data[index - context->globals.size].valType != global->valType){
                    wasmvm_errno = ERROR_type_mis;
                    return 0;
                }
            }else{
                if(context->globals.data[index] != global->valType){
                    wasmvm_errno = ERROR_type_mis;
                    return 0;
                }
            }
        }break;
        case Const_Value:
            if(global->init.value.type != global->valType){
                wasmvm_errno = ERROR_type_mis;
                return 0;
            }
        break;
        default:
            wasmvm_errno = ERROR_req_const_expr;
            return 0;
    }
    return 1;
}

_Bool elem_validate(WasmElem* elem, WasmModule* module, ValidateContext* context){
    // Type
    if(elem->type != Value_funcref && elem->type != Value_externref){
        wasmvm_errno = ERROR_type_mis;
        return 0;
    }
    // Inits
    for(size_t i = 0; i < elem->init.size; ++i){
        ConstExpr* init = elem->init.data + i;
        switch (init->type){
            case Const_GlobalIndex:
                if(init->value.value.u32 >= (context->globals.size + module->globals.size)){
                    wasmvm_errno = ERROR_unknown_global;
                    return 0;
                }
                if(init->value.value.u32 >= context->globals.size){
                    // Global
                    if(module->globals.data[init->value.value.u32 - context->globals.size].valType != elem->type){
                        wasmvm_errno = ERROR_type_mis;
                        return 0;
                    }
                }else{
                    // Import
                    if(context->globals.data[init->value.value.u32] != elem->type){
                        wasmvm_errno = ERROR_type_mis;
                        return 0;
                    }
                }
                break;
            case Const_Value:
                if(init->value.type != elem->type){
                    wasmvm_errno = ERROR_unknown_global;
                    return 0;
                }
                break;
            default:
                wasmvm_errno = ERROR_type_mis;
                return 0;
        }
    }
    // Mode
    if(elem->mode.mode == Elem_active){
        if(elem->mode.tableidx >= (context->tables.size + module->tables.size)){
            wasmvm_errno = ERROR_unknown_table;
            return 0;
        }
        switch (elem->mode.offset.type){
            case Const_GlobalIndex:
                if(elem->mode.offset.value.value.u32 >= (context->globals.size + module->globals.size)){
                    wasmvm_errno = ERROR_unknown_global;
                    return 0;
                }
                if(elem->mode.offset.value.value.u32 >= context->globals.size){
                    // Global
                    if(module->globals.data[elem->mode.offset.value.value.u32 - context->globals.size].valType != Value_i32){
                        wasmvm_errno = ERROR_type_mis;
                        return 0;
                    }
                }else{
                    // Import
                    if(context->globals.data[elem->mode.offset.value.value.u32] != Value_i32){
                        wasmvm_errno = ERROR_type_mis;
                        return 0;
                    }
                }
                break;
            case Const_Value:
                if(elem->mode.offset.value.type != Value_i32){
                    wasmvm_errno = ERROR_unknown_global;
                    return 0;
                }
                break;
            default:
                wasmvm_errno = ERROR_type_mis;
                return 0;
        }
    }
    return 1;
}