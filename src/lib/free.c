/**
 * Copyright 2021 Luis Hsu, Kevin Cyu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include <WasmVM.h>
#include <Opcodes.h>
#include <structures/WasmModule.h>
#include <dataTypes/vector_t.h>

static void free_FuncType(FuncType* type)
{
    free_func(type->params.data);
    vector_init(type->params);
    free_func(type->results.data);
    vector_init(type->results);
}

static void free_Import(WasmImport* import)
{
    free_func(import->module.data);
    vector_init(import->module);
    free_func(import->name.data);
    vector_init(import->name);
}

static void free_Export(WasmExport* export)
{
    free_func(export->name.data);
    vector_init(export->name);
}

static void free_Func(WasmFunc* func)
{
    // locals
    free_func(func->locals.data);
    vector_init(func->locals);
    // body
    for(unsigned int i = 0; i < func->body.size; ++i) {
        WasmInstr* instr = func->body.data + i;
        free_Instr(instr);
    }
    free_func(func->body.data);
    vector_init(func->body);
}

static void free_Data(WasmData* data)
{
    free_func(data->init.data);
    vector_init(data->init);
}

void free_Instr(WasmInstr* instr)
{
    switch (instr->opcode) {
        case Op_select_t:
        case Op_br_table:
            free_func(instr->imm.vec.data);
            break;
        default:
            break;
    }
}

void module_free(wasm_module modulePtr)
{
    if(modulePtr != NULL) {
        WasmModule* module = modulePtr;
        // types
        for(unsigned int i = 0; i < module->types.size; ++i) {
            free_FuncType(module->types.data + i);
        }
        free_vector(module->types);
        // imports
        for(unsigned int i = 0; i < module->imports.size; ++i) {
            free_Import(module->imports.data + i);
        }
        free_vector(module->imports);
        // funcs
        for(unsigned int i = 0; i < module->funcs.size; ++i) {
            free_Func(module->funcs.data + i);
        }
        free_vector(module->funcs);
        // tables
        free_vector(module->tables);
        // mems
        free_vector(module->mems);
        // globals
        free_vector(module->globals);
        // exports
        for(unsigned int i = 0; i < module->exports.size; ++i) {
            free_Export(module->exports.data + i);
        }
        free_vector(module->exports);
        // TODO: elems
        // datas
        for(unsigned int i = 0; i < module->datas.size; ++i) {
            free_Data(module->datas.data + i);
        }
        free_vector(module->datas);
        free_func(modulePtr);
        modulePtr = NULL;
    }
}