/**
 * Copyright 2021 Luis Hsu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include <error.h>
#include <WasmVM.h>

#include "instrs.h"
#include "../invoke/invoke.h"

struct module_import_sizes {
    size_t funcs;
    size_t globals;
    size_t mems;
    size_t tables;
};

static wasm_module_inst module_alloc(wasm_store store, const wasm_module module, externval_vector_t externval, struct module_import_sizes sizes)
{
    // Allocate vectors
    wasm_module_inst moduleInst = module_inst_create();
    sizes.funcs += module->funcs.size;
    if(sizes.funcs > 0){
        moduleInst->funcaddrs.size = sizes.funcs;
        vector_resize(moduleInst->funcaddrs, u32_t, moduleInst->funcaddrs.size);
    }
    sizes.globals += module->globals.size;
    if(sizes.globals > 0){
        moduleInst->globaladdrs.size = sizes.globals;
        vector_resize(moduleInst->globaladdrs, u32_t, moduleInst->globaladdrs.size);
    }
    sizes.tables += module->tables.size;
    if(sizes.tables > 0){
        moduleInst->tableaddrs.size = sizes.tables;
        vector_resize(moduleInst->tableaddrs, u32_t, moduleInst->tableaddrs.size);
    }
    sizes.mems += module->mems.size;
    if(sizes.mems > 0){
        moduleInst->memaddrs.size = sizes.mems;
        vector_resize(moduleInst->memaddrs, u32_t, moduleInst->memaddrs.size);
    }
    if(module->elems.size > 0){
        moduleInst->elemaddrs.size = module->elems.size;
        vector_resize(moduleInst->elemaddrs, u32_t, moduleInst->elemaddrs.size);
    }
    if(module->datas.size > 0){
        moduleInst->dataaddrs.size = module->datas.size;
        vector_resize(moduleInst->dataaddrs, u32_t, moduleInst->dataaddrs.size);
    }

    // Externvals
    struct module_import_sizes offsets = {
        .funcs = 0,
        .globals = 0,
        .mems = 0,
        .tables = 0
    };
    for(size_t i = 0; i < externval.size; ++i) {
        const ExternVal *extVal = externval.data + i;
        switch (extVal->type) {
            case Desc_Func:
                moduleInst->funcaddrs.data[offsets.funcs++] = extVal->value;
                break;
            case Desc_Global:
                moduleInst->globaladdrs.data[offsets.globals++] = extVal->value;
                break;
            case Desc_Table:
                moduleInst->tableaddrs.data[offsets.tables++] = extVal->value;
                break;
            case Desc_Mem:
                moduleInst->memaddrs.data[offsets.mems++] = extVal->value;
                break;
            default:
                wasmvm_errno = ERROR_unknown_type;
                return NULL;
        }
    }

    // Types
    if(module->types.size > 0){
        moduleInst->types.size = module->types.size;
        vector_resize(moduleInst->types, FuncType, moduleInst->types.size);
        for(size_t i = 0; i < moduleInst->types.size; ++i) {
            moduleInst->types.data[i].params.size = module->types.data[i].params.size;
            if(moduleInst->types.data[i].params.size > 0) {
                moduleInst->types.data[i].params.data = (ValueType*) malloc_func(sizeof(ValueType) * module->types.data[i].params.size);
                memcpy_func(moduleInst->types.data[i].params.data, module->types.data[i].params.data, sizeof(ValueType) * module->types.data[i].params.size);
            } else {
                moduleInst->types.data[i].params.data = NULL;
            }
            moduleInst->types.data[i].results.size = module->types.data[i].results.size;
            if(moduleInst->types.data[i].results.size > 0) {
                moduleInst->types.data[i].results.data = (ValueType*) malloc_func(sizeof(ValueType) * module->types.data[i].results.size);
                memcpy_func(moduleInst->types.data[i].results.data, module->types.data[i].results.data, sizeof(ValueType) * module->types.data[i].results.size);
            } else {
                moduleInst->types.data[i].results.data = NULL;
            }
        }
    }
    // Functions
    if(module->funcs.size > 0){
        // Allocate FuncInst
        vector_resize(store->funcs, FuncInst, store->funcs.size + module->funcs.size);
        // Fill FuncInst
        for(size_t i = 0; i < module->funcs.size; ++i) {
            const WasmFunc *func = module->funcs.data + i;
            FuncInst* funcInst = store->funcs.data + (store->funcs.size + i);
            funcInst->bodyType = FuncBody_Wasm;
            funcInst->type = moduleInst->types.data + func->type;
            funcInst->body.wasm.module = moduleInst;
            funcInst->body.wasm.locals.size = func->locals.size;
            if(funcInst->body.wasm.locals.size > 0){
                funcInst->body.wasm.locals.data = (ValueType*) malloc_func(sizeof(ValueType) * func->locals.size);
                memcpy_func(funcInst->body.wasm.locals.data, func->locals.data, sizeof(ValueType) * func->locals.size);
            }else{
                funcInst->body.wasm.locals.data = NULL;
            }
            size_t codeLen = get_code_size(func);
            funcInst->body.wasm.codes.size = codeLen;
            if(funcInst->body.wasm.codes.size){
                funcInst->body.wasm.codes.data = (byte_t*) malloc_func(codeLen);
                fill_func_body(func, funcInst->body.wasm.codes.data);
            }else{
                funcInst->body.wasm.codes.data = NULL;
            }
            if(wasmvm_errno != ERROR_success){
                module_inst_free(moduleInst);
                return NULL;
            }
            moduleInst->funcaddrs.data[offsets.funcs + i] = store->funcs.size + i;
        }
        store->funcs.size += module->funcs.size;
    }

    // Tables
    if(module->tables.size > 0){
        // Allocate TableInst
        vector_resize(store->tables, TableInst, store->tables.size + module->tables.size);
        // Fill TableInst
        for(size_t i = 0; i < module->tables.size; ++i) {
            const WasmTable *table = module->tables.data + i;
            TableInst* tableInst = store->tables.data + store->tables.size + i;
            tableInst->max = table->max;
            tableInst->type = table->refType;
            vector_init(tableInst->elem);
            if(table->min > 0) {
                tableInst->elem.size = table->min;
                vector_resize(tableInst->elem, Ref, table->min);
                for(unsigned int i = 0; i < tableInst->elem.size; ++i) {
                    tableInst->elem.data[i].type = table->refType;
                    tableInst->elem.data[i].isNull = 1;
                }
            }
            moduleInst->tableaddrs.data[offsets.tables + i] = store->tables.size + i;
        }
        store->tables.size += module->tables.size;
    }

    // Memories
    if(module->mems.size > 0){
        // Allocate MemInst
        vector_resize(store->mems, MemInst, store->mems.size + module->mems.size);
        // Fill MemInst
        for(size_t i = 0; i < module->mems.size; ++i){
            const WasmMemory *memory = module->mems.data + i;
            MemInst* memInst = store->mems.data + store->mems.size + i;
            memInst->max = memory->max;
            vector_init(memInst->data);
            if(memory->min > 0){
                memInst->data.size = page_size * memory->min;
                vector_resize(memInst->data, byte_t, memInst->data.size);
                memset_func(memInst->data.data, 0, memInst->data.size);
            }
            moduleInst->memaddrs.data[offsets.mems + i] = store->mems.size + i;
        }
        store->mems.size += module->mems.size;
    }

    // Globals
    if(module->globals.size > 0){
        // Allocate GlobalInst
        vector_resize(store->globals, GlobalInst, store->globals.size + module->globals.size);
        // Fill GlobalInst
        for(size_t i = 0; i < module->globals.size; ++i){
            const WasmGlobal *global = module->globals.data + i;
            GlobalInst* globalInst = store->globals.data + store->globals.size + i;
            globalInst->mut = global->mut;
            if(global->init.type == Const_Value){
                globalInst->val.type = global->valType;
                globalInst->val.value = global->init.value.value;
            }else{
                GlobalInst* fetched = store->globals.data + moduleInst->globaladdrs.data[global->init.value.value.u32];
                globalInst->val = fetched->val;
            }
            moduleInst->globaladdrs.data[offsets.globals + i] = store->globals.size + i;
        }
        store->globals.size += module->globals.size;
    }
    
    // Elements
    if(module->elems.size > 0){
        // Allocate ElemInst
        vector_resize(store->elems, ElemInst, store->elems.size + module->elems.size);
        // Fill ElemInst
        for(size_t i = 0; i < module->elems.size; ++i){
            const WasmElem *elem = module->elems.data + i;
            ElemInst* elemInst = store->elems.data + store->elems.size + i;
            if(elem->type != Value_funcref){
                wasmvm_errno = ERROR_unknown_type;
                module_inst_free(moduleInst);
                return NULL;
            }
            elemInst->type = Ref_func;
            elemInst->elem.size = elem->init.size;
            elemInst->elem.data = malloc_func(sizeof(Ref) * elemInst->elem.size);
            for(size_t j = 0; j < elemInst->elem.size; ++j){
                elemInst->elem.data[j].type = Ref_func;
                elemInst->elem.data[j].isNull = 0;
                elemInst->elem.data[j].addr = moduleInst->funcaddrs.data[elem->init.data[j].value.value.u32];
            }
            moduleInst->elemaddrs.data[i] = store->elems.size + i;
        }
        store->elems.size += module->elems.size;
    }
    
    // Datas
    if(module->datas.size > 0){
        // Allocate DataInst
        vector_resize(store->datas, DataInst, store->datas.size + module->datas.size);
        // Fill DataInst
        for(size_t i = 0; i < module->datas.size; ++i){
            const WasmData* data = module->datas.data + i;
            DataInst* dataInst = store->datas.data + store->datas.size + i;
            dataInst->data.size = data->init.size;
            dataInst->data.data = malloc_func(sizeof(byte_t) * dataInst->data.size);
            memcpy_func(dataInst->data.data, data->init.data, sizeof(byte_t) * dataInst->data.size);
            moduleInst->dataaddrs.data[i] = store->datas.size + i;
        }
        store->datas.size += module->datas.size;
    }
    
    // Exports
    if(module->exports.size > 0){
        // Allocate ExportInsts
        moduleInst->exports.size = module->exports.size;
        moduleInst->exports.data = malloc_func(sizeof(ExportInst) * module->exports.size);
        for(size_t i = 0; i < module->exports.size; ++i){
            moduleInst->exports.data[i].value.type = module->exports.data[i].descType;
            switch (module->exports.data[i].descType){
            case Desc_Func:
                moduleInst->exports.data[i].value.value = moduleInst->funcaddrs.data[module->exports.data[i].descIdx];
                break;
            case Desc_Table:
                moduleInst->exports.data[i].value.value = moduleInst->tableaddrs.data[module->exports.data[i].descIdx];
                break;
            case Desc_Mem:
                moduleInst->exports.data[i].value.value = moduleInst->memaddrs.data[module->exports.data[i].descIdx];
                break;
            case Desc_Global:
                moduleInst->exports.data[i].value.value = moduleInst->globaladdrs.data[module->exports.data[i].descIdx];
                break;
            default:
                break;
            }
            moduleInst->exports.data[i].name.size = module->exports.data[i].name.size;
            moduleInst->exports.data[i].name.data = malloc_func(sizeof(byte_t) * module->exports.data[i].name.size);
            memcpy_func(moduleInst->exports.data[i].name.data, module->exports.data[i].name.data, sizeof(byte_t) * module->exports.data[i].name.size);
        }
    }
    return moduleInst;
}

wasm_module_inst module_instantiate(wasm_store store, const wasm_module module, externval_vector_t externval)
{
    // Check import number with externval
    if(module->imports.size != externval.size) {
        wasmvm_errno = ERROR_unknown_import;
        return NULL;
    }
    // Check externval
    struct module_import_sizes sizes = {
        .funcs = 0,
        .globals = 0,
        .mems = 0,
        .tables = 0
    };
    for(size_t i = 0; i < externval.size; ++i) {
        ExternVal *extVal = externval.data + i;
        switch (extVal->type) {
            case Desc_Func:
                if(extVal->value >= store->funcs.size) {
                    wasmvm_errno = ERROR_len_out_of_bound;
                    return NULL;
                }
                sizes.funcs += 1;
                break;
            case Desc_Global:
                if(extVal->value >= store->globals.size) {
                    wasmvm_errno = ERROR_len_out_of_bound;
                    return NULL;
                }
                sizes.globals += 1;
                break;
            case Desc_Table:
                if(extVal->value >= store->tables.size) {
                    wasmvm_errno = ERROR_len_out_of_bound;
                    return NULL;
                }
                sizes.tables += 1;
                break;
            case Desc_Mem:
                if(extVal->value >= store->mems.size) {
                    wasmvm_errno = ERROR_len_out_of_bound;
                    return NULL;
                }
                sizes.mems += 1;
                break;
            default:
                wasmvm_errno = ERROR_unknown_type;
                return NULL;
                break;
        }
        if(extVal->type != module->imports.data[i].descType) {
            wasmvm_errno = ERROR_type_mis;
            return NULL;
        }
    }
    // Allocate module instance
    wasm_module_inst moduleInst = module_alloc(store, module, externval, sizes);
    
    // Init tables with elems
    for(size_t i = 0; i < module->elems.size; ++i){
        const WasmElem* elem = module->elems.data + i;
        if(elem->mode.mode == Elem_active){
            TableInst *tableInst = store->tables.data + moduleInst->tableaddrs.data[elem->mode.tableidx];
            u32_t offset = elem->mode.offset.value.value.u32;
            if((offset + elem->init.size) > tableInst->elem.size){
                wasmvm_errno = ERROR_len_out_of_bound;
                module_inst_free(moduleInst);
                return NULL;
            }
            const ElemInst* elemInst = store->elems.data + moduleInst->elemaddrs.data[i];
            memcpy_func(tableInst->elem.data + offset, elemInst->elem.data, sizeof(Ref) * elemInst->elem.size);
        }
    }
    
    // Init memories with datas
    for(size_t i = 0; i < module->datas.size; ++i){
        const WasmData* data = module->datas.data + i;
        if(data->mode.mode == Data_active){
            MemInst *memInst = store->mems.data + moduleInst->memaddrs.data[data->mode.memidx];
            u32_t offset = data->mode.offset.value.value.u32;
            if((offset + data->init.size) > memInst->data.size){
                wasmvm_errno = ERROR_len_out_of_bound;
                module_inst_free(moduleInst);
                return NULL;
            }
            const DataInst* dataInst = store->datas.data + moduleInst->dataaddrs.data[i];
            memcpy_func(memInst->data.data + offset, dataInst->data.data, sizeof(byte_t) * dataInst->data.size);
        }
    }
    
    // Execute start function
    if(module->start != -1){
        if((module->start >= moduleInst->funcaddrs.size) || (moduleInst->funcaddrs.data[module->start] >= store->funcs.size)){
            wasmvm_errno = ERROR_unknown_func;
            module_inst_free(moduleInst);
            return NULL;
        }
        const u32_t funcAddr = moduleInst->funcaddrs.data[module->start];
        const FuncInst* funcInst = store->funcs.data + funcAddr;
        // Start shouldn't have params & results
        if(funcInst->type->params.size || funcInst->type->results.size){
            wasmvm_errno = ERROR_type_mis;
            module_inst_free(moduleInst);
            return NULL;
        }
        // Create Stack
        wasm_stack stack = NULL;
        invoke(&stack, store, funcAddr);
        execute(&stack, store);
        if(wasmvm_errno != ERROR_success){
            module_inst_free(moduleInst);
            return NULL;
        }
    }

    return moduleInst;
}