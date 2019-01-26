#include "Validates.h"

#include <stdint.h>
#include <stddef.h>
#include <string.h>

int validate_FunctionType(FuncType* funcType)
{
    return 0; // Unleash result type restrict
}

int validate_Module(WasmModule* module)
{
    int result = 0;
    for(uint32_t i = 0; i < module->types->length; ++i) {
        if((result = validate_FunctionType((FuncType*) module->types->at(module->types, i)))) {
            return result;
        }
    }
    for(uint32_t i = 0; i < module->funcs->length; ++i) {
        if((result = validate_Func((WasmFunc*) module->funcs->at(module->funcs, i), module))) {
            return result;
        }
    }
    for(uint32_t i = 0; i < module->tables->length; ++i) {
        if((result = validate_Table((WasmTable*) module->tables->at(module->tables, i)))) {
            return result;
        }
    }
    for(uint32_t i = 0; i < module->mems->length; ++i) {
        if((result = validate_Memory((WasmMemory*) module->mems->at(module->mems, i)))) {
            return result;
        }
    }
    for(uint32_t i = 0; i < module->globals->length; ++i) {
        if((result = validate_Global((WasmGlobal*) module->globals->at(module->globals, i)))) {
            return result;
        }
    }
    for(uint32_t i = 0; i < module->elems->length; ++i) {
        if((result = validate_Elem((WasmElem*) module->elems->at(module->elems, i), module))) {
            return result;
        }
    }
    for(uint32_t i = 0; i < module->datas->length; ++i) {
        if((result = validate_Data((WasmData*) module->datas->at(module->datas, i), module))) {
            return result;
        }
    }
    // Start
    if(module->start >= module->funcs->length) {
        return -1;
    }
    FuncType* startFuncType = (FuncType*)module->types->at(module->types, ((WasmFunc*)module->funcs->at(module->funcs, module->start))->type);
    if(startFuncType->params->length != 0 || startFuncType->results->length != 0) {
        return -2;
    }

    for(uint32_t i = 0; i < module->imports->length; ++i) {
        if((result = validate_Import((WasmImport*) module->imports->at(module->imports, i), module))) {
            return result;
        }
    }
    for(uint32_t i = 0; i < module->exports->length; ++i) {
        WasmExport* export = (WasmExport*)module->exports->at(module->exports, i);
        if((result = validate_Export(export, module))) {
            return result;
        }
        // Export name
        for(uint32_t j = 0; j < i; ++j) {
            WasmExport* former = (WasmExport*)module->exports->at(module->exports, j);
            if(!strcmp(former->name, export->name)) {
                return -3;
            }
        }
    }
    return result;
}

int validate_Func(WasmFunc* func, WasmModule* module)
{
    if(func->type >= module->types->length) {
        return -1;
    }
    Context* context = new_Context(module, func);
    int result = validate_Expr(func->body, context);
    free_Context(context);
    return result;
}
int validate_Table(WasmTable* table)
{
    return !(table->min < UINT32_MAX && ((table->max > 0) ? table->max < UINT32_MAX && table->max >= table->min : 1));
}
int validate_Memory(WasmMemory* memory)
{
    return !(memory->min < UINT16_MAX && ((memory->max > 0) ? memory->max < UINT16_MAX && memory->max >= memory->min : 1));
}
int validate_Global(WasmGlobal* global)
{
    // 1. The global type is valid
    // 2. Constant expr has been expand to value
    return 0;
}
int validate_Elem(WasmElem* elem, WasmModule* module)
{
    int result = elem->table < module->tables->length;
    result &= (elem->offset.type == Value_i32);
    for(size_t i = 0; i < elem->init->length; ++i) {
        uint32_t* funcIndex = (uint32_t*)elem->init->at(elem->init, i);
        result &= *funcIndex < module->funcs->length;
    }
    return !result;
}
int validate_Data(WasmData* data, WasmModule* module)
{
    int result = data->data < module->mems->length;
    result &= (data->offset.type == Value_i32);
    return !result;
}
int validate_Export(WasmExport* exports, WasmModule* module)
{
    switch(exports->descType) {
        case Desc_Func:
            return !(exports->descIdx < module->funcs->length);
        case Desc_Global:
            return !(exports->descIdx < module->globals->length);
        case Desc_Mem:
            return !(exports->descIdx < module->mems->length);
        case Desc_Table:
            return !(exports->descIdx < module->tables->length);
        default:
            return -1;
    }
}
int validate_Import(WasmImport* imports, WasmModule* module)
{
    switch(imports->descType) {
        case Desc_Func:
            return !(imports->desc.typeidx < module->types->length);
        case Desc_Global:
            return 0;
        case Desc_Mem:
            return !(imports->desc.limits.min < UINT16_MAX && ((imports->desc.limits.max > 0) ? imports->desc.limits.max < UINT16_MAX && imports->desc.limits.max >= imports->desc.limits.min : 1));
        case Desc_Table:
            return !(imports->desc.limits.min < UINT32_MAX && ((imports->desc.limits.max > 0) ? imports->desc.limits.max < UINT32_MAX && imports->desc.limits.max >= imports->desc.limits.min : 1));
        default:
            return -1;
    }
}