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
    newModule->types = new_vector_p(struct FuncType_, (void(*)(void*))clean_FuncType);
    newModule->funcs = new_vector_p(WasmFunc, (void(*)(void*))clean_WasmFunc);
    newModule->tables = new_vector_p(WasmTable, NULL);
    newModule->mems = new_vector_p(WasmMemory, NULL);
    newModule->globals = new_vector_p(WasmGlobal, NULL);
    newModule->elems = new_vector_p(WasmElem, (void(*)(void*))clean_WasmElem);
    newModule->datas = new_vector_p(WasmData, (void(*)(void*))clean_WasmData);
    newModule->imports = new_vector_p(WasmImport, (void(*)(void*))clean_WasmImport);
    newModule->exports = new_vector_p(WasmExport, (void(*)(void*))clean_WasmExport);
    return newModule;
}

void clean_WasmModule(WasmModule* module)
{
    if(module) {
        free(module->module_name);
        free_vector_p(module->types);
        free_vector_p(module->funcs);
        free_vector_p(module->tables);
        free_vector_p(module->mems);
        free_vector_p(module->globals);
        free_vector_p(module->elems);
        free_vector_p(module->datas);
        free_vector_p(module->imports);
        free_vector_p(module->exports);
    }
}

void free_WasmModule(WasmModule* module)
{
    clean_WasmModule(module);
    free(module);
}
