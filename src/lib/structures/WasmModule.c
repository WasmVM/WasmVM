#include <structures/WasmModule.h>

#include <stdlib.h>
#include <string.h>

WasmModule* new_WasmModule(char* module_name)
{
    WasmModule* newModule = (WasmModule*) malloc(sizeof(WasmModule));
    // construct
    if(module_name) {
        newModule->module_name = (char*)malloc(sizeof(char) * (strlen(module_name) + 1));
        strcpy(newModule->module_name, module_name);
    } else {
        newModule->module_name = NULL;
    }
    newModule->types = new_vector(sizeof(FuncType), (void(*)(void*))clean_FuncType);
    newModule->funcs = new_vector(sizeof(WasmFunc), (void(*)(void*))clean_WasmFunc);
    newModule->tables = new_vector(sizeof(WasmTable), NULL);
    newModule->mems = new_vector(sizeof(WasmMemory), NULL);
    newModule->globals = new_vector(sizeof(WasmGlobal), NULL);
    newModule->elems = new_vector(sizeof(WasmElem), (void(*)(void*))clean_WasmElem);
    newModule->datas = new_vector(sizeof(WasmData), (void(*)(void*))clean_WasmData);
    newModule->imports = new_vector(sizeof(WasmImport), (void(*)(void*))clean_WasmImport);
    newModule->exports = new_vector(sizeof(WasmExport), (void(*)(void*))clean_WasmExport);
    return newModule;
}

void clean_WasmModule(WasmModule* module)
{
    if(module) {
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
    }
}

void free_WasmModule(WasmModule* module)
{
    clean_WasmModule(module);
    free(module);
}
