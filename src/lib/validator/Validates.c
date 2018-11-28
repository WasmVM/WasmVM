#include "Validates.h"

#include <stdint.h>
#include <stddef.h>

int validate_FunctionType(FuncType* funcType)
{
    return 0; // Unleash result type restrict
}

int validate_Func(WasmFunc* func, WasmModule* module) // FIXME: Test
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
int validate_FunctionType(FuncType* funcType)
{
    return !(funcType->results->length <= 1); // TODO: Unleash later
}

int validate_Func(WasmFunc* func)
{

}
int validate_Expr(vector* expr)
{
    return 0; // TODO:
}
int validate_Table(WasmTable* table)
{
    return !(table->min < UINT32_MAX && ((table->max > 0) ? table->max < UINT32_MAX && table->max >= table->min : 1));
}
int validate_Memory(WasmMemory* memory) //FIXME: test
{
    return !(memory->min < UINT16_MAX && ((memory->max > 0) ? memory->max < UINT16_MAX && memory->max >= memory->min : 1));
}
int validate_Global(WasmGlobal* global) //FIXME: test
{
    // 1. The global type is valid
    // 2. Constant expr has been expand to value
    return 0;
}
int validate_Elem(WasmElem* elem, WasmModule* module) //FIXME: test
{
    int result = elem->table < module->tables->length;
    result &= (elem->offset.type == Value_i32);
    for(size_t i = 0; i < elem->init->length; ++i) {
        result &= *((uint32_t*)elem->init->at(elem->init, i)) < module->funcs->length;
    }
    return !result;
}
int validate_Data(WasmData* data, WasmModule* module) //FIXME: test
{
    int result = data->data < module->mems->length;
    result &= (data->offset.type == Value_i32);
    return !result;
}
int validate_Export(WasmExport* exports, WasmModule* module) //FIXME: test
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
int validate_Import(WasmImport* imports, WasmModule* module) //FIXME: test
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
