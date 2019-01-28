#include <structures/WasmData.h>

#include <stdlib.h>

WasmData* new_WasmData()
{
    WasmData* newWasmData = (WasmData*) malloc(sizeof(WasmData));
    newWasmData->init = new_vector(sizeof(char), NULL);

    return newWasmData;
}

void clean_WasmData(WasmData* thisWasmData)
{
    free_vector(thisWasmData->init);
}

void free_WasmData(WasmData* thisWasmData)
{
    clean_WasmData(thisWasmData);
    free(thisWasmData);
}
