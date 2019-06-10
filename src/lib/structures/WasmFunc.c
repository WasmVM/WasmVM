#include <structures/WasmFunc.h>

#include <stdlib.h>

WasmFunc* new_WasmFunc()
{
    WasmFunc* newWasmFunc = (WasmFunc*) malloc(sizeof(WasmFunc));
    // vectors
    newWasmFunc->locals = new_vector_p(ValueType, NULL);
    newWasmFunc->body = new_list_p(free_WasmInstr);

    return newWasmFunc;
}

void clean_WasmFunc(WasmFunc* thisWasmFunc)
{
    if(thisWasmFunc) {
        free_vector_p(thisWasmFunc->locals);
        free_list_p(thisWasmFunc->body);
    }
}

void free_WasmFunc(WasmFunc* thisWasmFunc)
{
    clean_WasmFunc(thisWasmFunc);
    free(thisWasmFunc);
}
