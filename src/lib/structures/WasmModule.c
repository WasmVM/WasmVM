#include <structures/WasmModule.h>

#include <stdlib.h>

WasmModule* new_WasmModule(char* module_name)
{
    WasmModule* newModule = (WasmModule*) malloc(sizeof(WasmModule));
    // construct
    newModule->module_name = module_name;
    newModule->types = new_vector(sizeof(FuncType));
    newModule->funcs = new_vector(sizeof(WasmFunc));
    newModule->tables = new_vector(sizeof(WasmTable));
    newModule->mems = new_vector(sizeof(WasmMemory));
    newModule->globals = new_vector(sizeof(WasmGlobal));
    newModule->elems = new_vector(sizeof(WasmElem));
    newModule->datas = new_vector(sizeof(WasmData));
    newModule->imports = new_vector(sizeof(WasmImport));
    newModule->exports = new_vector(sizeof(WasmExport));

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
