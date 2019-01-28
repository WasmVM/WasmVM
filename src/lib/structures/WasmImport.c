#include <structures/WasmImport.h>

#include <stdlib.h>

WasmImport* new_WasmImport(char* moduleName, char* name)
{
    WasmImport* import = (WasmImport*) malloc(sizeof(WasmImport));
    import->module = moduleName;
    import->name = name;
    return import;
}
void clean_WasmImport(WasmImport* import)
{
    if(import) {
        free(import->name);
        free(import->module);
    }
}
void free_WasmImport(WasmImport* import)
{
    clean_WasmImport(import);
    free(import);
}