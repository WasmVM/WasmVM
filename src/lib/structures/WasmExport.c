#include <structures/WasmExport.h>

#include <stdlib.h>

WasmExport* new_WasmExport(char* name, DescType descType, uint32_t descIdx)
{
    WasmExport* newExport = (WasmExport*) malloc(sizeof(WasmExport));
    newExport->name = name;
    newExport->descType = descType;
    newExport->descIdx = descIdx;
    return newExport;
}
void clean_WasmExport(WasmExport *thisWasmExport)
{
    if(thisWasmExport) {
        free(thisWasmExport->name);
    }
}
void free_WasmExport(WasmExport *thisWasmExport)
{
    clean_WasmExport(thisWasmExport);
    free(thisWasmExport);
}