#include <structures/WasmElem.h>

WasmElem* new_WasmElem(){
    WasmElem* newWasmElem = (WasmElem*) malloc(sizeof(WasmElem));
    newWasmElem->init = new_vector(sizeof(uint32_t), (void(*)(void*)free));

    return newWasmElem;
}

void free_WasmElem(WasmElem *thisWasmElem){
    free(thisWasmElem->init);
    free(thisWasmElem);
}