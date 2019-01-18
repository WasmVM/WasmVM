#include <structures/WasmData.h>

#include <stdlib.h>

WasmData* new_WasmData()
{
    WasmData* newWasmData = (WasmData*) malloc(sizeof(WasmData));
    newWasmData->init = new_vector(sizeof(char));

    return newWasmData;
}
void free_WasmData(WasmData* thisWasmData)
{
    free_vector(thisWasmData->init);
    free(thisWasmData);
}
