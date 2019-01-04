#include <structures/WasmModule.h>

#include <stdlib.h>

WasmModule* new_WasmModule(char* module_name)
{
    WasmModule* newModule = (WasmModule*) malloc(sizeof(WasmModule));
    // construct
    newModule->module_name = module_name;
    newModule->types = new_vector(sizeof(FuncType), (void(*)(void*))free_FuncType);
    newModule->funcs = new_vector(sizeof(WasmFunc), (void(*)(void*))free_WasmFunc);
    newModule->tables = new_vector(sizeof(WasmTable), (void(*)(void*))free);
    newModule->mems = new_vector(sizeof(WasmMemory), (void(*)(void*))free);
    newModule->globals = new_vector(sizeof(WasmGlobal), (void(*)(void*))free);
    newModule->elems = new_vector(sizeof(WasmElem), (void(*)(void*))free_WasmElem);
    newModule->datas = new_vector(sizeof(WasmData), (void(*)(void*))free_WasmData);
    newModule->imports = new_vector(sizeof(WasmImport), (void(*)(void*))free);
    newModule->exports = new_vector(sizeof(WasmExport), (void(*)(void*))free);

    return newModule;
}

void free_WasmModule(WasmModule* module)
{
    free(module->module_name);
    free_vector(module->types);
    free_vector(module->funcs);
    free_vector(module->tables);
    free_vector(module->mems);
    free_vector(module->globals);
    free_vector(module->elems);
    free_vector(module->datas);
    free_vector(module->imports);
    free_vector(module->exports);
    free(module);
}