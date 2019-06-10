#include <structures/WasmElem.h>
#include <dataTypes/Value.h>

#include <stdlib.h>

WasmElem* new_WasmElem()
{
    WasmElem* newWasmElem = (WasmElem*) malloc(sizeof(WasmElem));
    newWasmElem->table = 0;
    newWasmElem->init = new_vector_p(uint32_t, NULL);
    newWasmElem->offset.parent.entryType = Entry_Value;
    newWasmElem->offset.type = Value_i32;
    newWasmElem->offset.value.i32 = 0;
    return newWasmElem;
}

void clean_WasmElem(WasmElem *thisWasmElem)
{
    free_vector_p(thisWasmElem->init);
}

void free_WasmElem(WasmElem *thisWasmElem)
{
    clean_WasmElem(thisWasmElem);
    free(thisWasmElem);
}
