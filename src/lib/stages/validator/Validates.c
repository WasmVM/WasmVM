#include "Validates.h"

#include <stdint.h>
#include <stddef.h>
#include <string.h>

int validate_FunctionType(FuncType funcType)
{
    return 0; // Unleash result type restrict
}

int validate_Module(WasmModule* module)
{
    int result = 0;
    for(uint32_t i = 0; i < vector_size(module->types); ++i) {
        if((result = validate_FunctionType(vector_at(FuncType, module->types, i)))) {
            return result;
        }
    }
    for(uint32_t i = 0; i < vector_size(module->funcs); ++i) {
        if((result = validate_Func(vector_at(WasmFunc*, module->funcs, i), module))) {
            return result;
        }
    }
    for(uint32_t i = 0; i < vector_size(module->tables); ++i) {
        if((result = validate_Table(vector_at(WasmTable*, module->tables, i)))) {
            return result;
        }
    }
    for(uint32_t i = 0; i < vector_size(module->mems); ++i) {
        if((result = validate_Memory(vector_at(WasmMemory*, module->mems, i)))) {
            return result;
        }
    }
    for(uint32_t i = 0; i < vector_size(module->globals); ++i) {
        if((result = validate_Global(vector_at(WasmGlobal*, module->globals, i)))) {
            return result;
        }
    }
    for(uint32_t i = 0; i < vector_size(module->elems); ++i) {
        if((result = validate_Elem(vector_at(WasmElem*, module->elems, i), module))) {
            return result;
        }
    }
    for(uint32_t i = 0; i < vector_size(module->datas); ++i) {
        if((result = validate_Data(vector_at(WasmData*, module->datas, i), module))) {
            return result;
        }
    }

    // Validate start function
    if(module->start >= vector_size(module->funcs)) {
        return -1;
    }
    FuncType startFuncType = vector_at(FuncType, module->types, (vector_at(WasmFunc*, module->funcs, module->start))->type);
    if(vector_size(startFuncType->params) != 0 || vector_size(startFuncType->results) != 0) {
        return -2;
    }

    for(uint32_t i = 0; i < vector_size(module->imports); ++i) {
        if((result = validate_Import(vector_at(WasmImport*, module->imports, i), module))) {
            return result;
        }
    }
    for(uint32_t i = 0; i < vector_size(module->exports); ++i) {
        WasmExport* export = vector_at(WasmExport*, module->exports, i);
        if((result = validate_Export(export, module))) {
            return result;
        }
        // Export name
        for(uint32_t j = 0; j < i; ++j) {
            WasmExport* former = vector_at(WasmExport*, module->exports, j);
            if(!strcmp(former->name, export->name)) {
                return -3;
            }
        }
    }
    return result;
}

int validate_Func(WasmFunc* func, WasmModule* module)
{
    if(func->type >= vector_size(module->types)) {
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
    int result = elem->table < vector_size(module->tables);
    result &= (elem->offset.type == Value_i32);
    for(size_t i = 0; i < vector_size(elem->init); ++i) {
        uint32_t* funcIndex = vector_at(uint32_t*, elem->init, i);
        result &= *funcIndex < vector_size(module->funcs);
    }
    return !result;
}
int validate_Data(WasmData* data, WasmModule* module)
{
    int result = data->data < vector_size(module->mems);
    result &= (data->offset.type == Value_i32);
    return !result;
}
int validate_Export(WasmExport* exports, WasmModule* module)
{
    switch(exports->descType) {
        case Desc_Func:
            return !(exports->descIdx < vector_size(module->funcs));
        case Desc_Global:
            return !(exports->descIdx < vector_size(module->globals));
        case Desc_Mem:
            return !(exports->descIdx < vector_size(module->mems));
        case Desc_Table:
            return !(exports->descIdx < vector_size(module->tables));
        default:
            return -1;
    }
}
int validate_Import(WasmImport* imports, WasmModule* module)
{
    switch(imports->descType) {
        case Desc_Func:
            return !(imports->desc.typeidx < vector_size(module->types));
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