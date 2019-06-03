#include <structures/WasmFunc.h>

#include <stdlib.h>

WasmFunc* new_WasmFunc()
{
    WasmFunc* newWasmFunc = (WasmFunc*) malloc(sizeof(WasmFunc));
    // vectors
    newWasmFunc->locals = new_vector(sizeof(ValueType), NULL);
    newWasmFunc->body = new_list_t(free_WasmInstr);

    return newWasmFunc;
}

void clean_WasmFunc(WasmFunc* thisWasmFunc)
{
    if(thisWasmFunc) {
        free_vector(thisWasmFunc->locals);
        free_list_t(thisWasmFunc->body);
    }
}

void free_WasmFunc(WasmFunc* thisWasmFunc)
{
    clean_WasmFunc(thisWasmFunc);
    free(thisWasmFunc);
}
