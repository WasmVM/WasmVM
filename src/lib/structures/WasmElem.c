#include <structures/WasmElem.h>
#include <dataTypes/Value.h>

#include <stdlib.h>

WasmElem* new_WasmElem()
{
    WasmElem* newWasmElem = (WasmElem*) malloc(sizeof(WasmElem));
    newWasmElem->table = 0;
    newWasmElem->init = new_vector(sizeof(uint32_t));
    newWasmElem->offset.parent.entryType = Entry_Value;
    newWasmElem->offset.type = Value_i32;
    newWasmElem->offset.value.i32 = 0;
    return newWasmElem;
}

void free_WasmElem(WasmElem *thisWasmElem)
{
    free_vector(thisWasmElem->init);
    free(thisWasmElem);
}
