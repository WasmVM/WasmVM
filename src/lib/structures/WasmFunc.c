#include <structures/WasmFunc.h>

WasmFunc* new_WasmFunc()
{
    WasmFunc* newWasmFunc = (WasmFunc*) malloc(sizeof(WasmFunc));
    // vectors
    newWasmFunc->locals = new_vector(sizeof(ValueType), (void(*)(void*))free);
    newWasmFunc->body = new_vector(sizeof(WasmInstr), (void(*)(void*))free);

    return newWasmFunc;
}

void free_WasmFunc(WasmFunc* thisWasmFunc)
{
    free_vector(thisWasmFunc->locals);
    free_vector(thisWasmFunc->body);
    free(thisWasmFunc);
}