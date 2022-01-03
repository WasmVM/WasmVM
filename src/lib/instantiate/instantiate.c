/**
 * Copyright 2021 Luis Hsu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include <error.h>
#include <WasmVM.h>

struct module_alloc_sizes {
    size_t funcs;
    size_t globals;
    size_t mems;
    size_t tables;
};

static wasm_module_inst module_alloc(wasm_store store, const wasm_module module, externval_vector_t externval, struct module_alloc_sizes sizes)
{
    // Allocate vectors
    wasm_module_inst moduleInst = module_inst_create();
    moduleInst->funcaddrs.size = sizes.funcs + module->funcs.size;
    vector_resize(moduleInst->funcaddrs, u32_t, moduleInst->funcaddrs.size);
    moduleInst->globaladdrs.size = sizes.globals + module->globals.size;
    vector_resize(moduleInst->globaladdrs, u32_t, moduleInst->globaladdrs.size);
    moduleInst->tableaddrs.size = sizes.tables + module->tables.size;
    vector_resize(moduleInst->tableaddrs, u32_t, moduleInst->tableaddrs.size);
    moduleInst->memaddrs.size = sizes.mems + module->mems.size;
    vector_resize(moduleInst->memaddrs, u32_t, moduleInst->memaddrs.size);

    // Externvals
    struct module_alloc_sizes offsets = {
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
    // Functions
    // Allocate FuncInst
    vector_resize(store->funcs, FuncInst, store->funcs.size + module->funcs.size);
    // Fill FuncInst
    for(size_t i = 0; i < module->funcs.size; ++i) {
        const WasmFunc *func = module->funcs.data + i;
        FuncInst* funcInst = store->funcs.data + (store->funcs.size + i);
        funcInst->bodyType = FuncBody_Wasm;
        funcInst->type = module->types.data + func->type;
        // TODO: Body
    }
    store->funcs.size += module->funcs.size;

    // Tables
    // Allocate TableInst
    vector_resize(store->tables, TableInst, store->tables.size + module->tables.size);
    // Fill TableInst
    for(size_t i = 0; i < module->tables.size; ++i) {
        const WasmTable *table = module->tables.data + i;
        TableInst* tableInst = store->tables.data + store->tables.size + i;
        tableInst->max = table->max;
        vector_init(tableInst->elem);
        if(table->min > 0) {
            tableInst->elem.size = table->min;
            vector_resize(tableInst->elem, Ref, table->min);
            for(unsigned int i = 0; i < tableInst->elem.size; ++i) {
                tableInst->elem.data[i].type = table->refType;
                tableInst->elem.data[i].isNull = 1;
            }
        }
        moduleInst->tableaddrs.data[i] = store->tables.size + i;
    }
    store->tables.size += module->tables.size;

    // Memories
    // Allocate MemInst
    vector_resize(store->mems, MemInst, store->mems.size + module->mems.size);
    // Fill MemInst
    for(size_t i = 0; i < module->mems.size; ++i){
        const WasmMemory *memory = module->mems.data + i;
        MemInst* memInst = store->mems.data + store->mems.size + i;
        memInst->max = memory->max;
        vector_init(memInst->data);
        if(memory->min > 0){
            memInst->data.size = 65536 * memory->min;
            vector_resize(memInst->data, byte_t, memInst->data.size);
            memset_func(memInst->data.data, 0, memInst->data.size);
        }
        moduleInst->memaddrs.data[i] = store->mems.size + i;
    }
    store->mems.size += module->mems.size;

    // Globals
    // Allocate GlobalInst
    vector_resize(store->globals, GlobalInst, store->globals.size + module->globals.size);
    // Fill GlobalInst
    for(size_t i = 0; i < module->globals.size; ++i){
        const WasmGlobal *global = module->globals.data + i;
        GlobalInst* globalInst = store->globals.data + store->globals.size + i;
        globalInst->val.type = global->valType;
        globalInst->mut = global->mut;
        globalInst->val.value = global->init.value.value;
        moduleInst->globaladdrs.data[i] = store->globals.size + i;
    }
    store->globals.size += module->globals.size;
    // Elements
    // Allocate ElemInst
    vector_resize(store->elems, ElemInst, store->elems.size + module->elems.size);
    // Fill ElemInst
    for(size_t i = 0; i < module->elems.size; ++i){
        const WasmElem *elem = module->elems.data + i;
        ElemInst* elemInst = store->elems.data + store->elems.size + i;
        if(elem->type != Value_funcref){
            wasmvm_errno = ERROR_unknown_type;
            return NULL;
        }
        elemInst->type = Ref_func;
        elemInst->elem.size = elem->init.size;
        vector_resize(elemInst->elem, Ref, elemInst->elem.size);
        for(size_t j = 0; j < elemInst->elem.size; ++j){
            elemInst->elem.data[j].type = Ref_func;
            elemInst->elem.data[j].isNull = 0;
            elemInst->elem.data[j].addr = elem->init.data[j].value.value.u32;
        }
        moduleInst->elemaddrs.data[i] = store->elems.size + i;
    }
    store->elems.size += module->elems.size;

    // Datas
    // Allocate DataInst
    vector_resize(store->datas, DataInst, store->datas.size + module->datas.size);
    // Fill DataInst
    for(size_t i = 0; i < module->datas.size; ++i){
        const WasmData* data = module->datas.data + i;
        DataInst* dataInst = store->datas.data + store->datas.size + i;
        dataInst->data.size = data->init.size;
        vector_resize(dataInst->data, byte_t, dataInst->data.size);
        memcpy_func(dataInst->data.data, data->init.data, sizeof(byte_t) * dataInst->data.size);
        moduleInst->dataaddrs.data[i] = store->elems.size + i;
    }
    store->datas.size += module->datas.size;
    // Exports
    // Allocate ExportInsts
    moduleInst->exports.size = module->exports.size;
    vector_resize(moduleInst->exports, ExportInst, module->exports.size);
    for(size_t i = 0; i < module->exports.size; ++i){
        moduleInst->exports.data[i] = *((ExportInst*)(module->exports.data + i));
        moduleInst->exports.data[i].name.data = malloc_func(sizeof(byte_t) * module->exports.data->name.size);
        memcpy_func(moduleInst->exports.data[i].name.data, module->exports.data->name.data, sizeof(byte_t) * module->exports.data->name.size);
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
    struct module_alloc_sizes sizes = {
        .funcs = 0,
        .globals = 0,
        .mems = 0,
        .tables = 0
    };
    for(size_t i = 0; i < externval.size; ++i) {
        ExternVal *extVal = externval.data + i;
        switch (extVal->type) {
            case Desc_Func:
                if(extVal->value >= module->funcs.size) {
                    wasmvm_errno = ERROR_len_out_of_bound;
                    return NULL;
                }
                sizes.funcs += 1;
                break;
            case Desc_Global:
                if(extVal->value >= module->globals.size) {
                    wasmvm_errno = ERROR_len_out_of_bound;
                    return NULL;
                }
                sizes.globals += 1;
                break;
            case Desc_Table:
                if(extVal->value >= module->tables.size) {
                    wasmvm_errno = ERROR_len_out_of_bound;
                    return NULL;
                }
                sizes.tables += 1;
                break;
            case Desc_Mem:
                if(extVal->value >= module->mems.size) {
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
    // TODO: Init tables with elems
    // TODO: Init memories with datas
    // TODO: Execute start function
    return moduleInst;
}