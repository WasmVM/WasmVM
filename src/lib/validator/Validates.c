#include "Validates.h"

#include <stdint.h>

int validate_FunctionType(FuncType* funcType)
{
    return !(funcType->results->length <= 1); // TODO: Unleash later
}

int validate_Func(WasmFunc* func)
{

}
int validate_Expr(vector* expr)
{

}
int validate_Table(WasmTable* table)
{
    uint32_t k = UINT32_MAX / 65536;
    return !(table->min < k && ((table->max > 0) ? table->max < k && table->max >= table->min : 1));
}
int validate_Memory(WasmMemory* memory)
{

}
int validate_Global(WasmGlobal* global)
{

}
int validate_Elem(WasmElem* elem)
{

}
int validate_Data(WasmData* data)
{

}
int validate_Export(WasmExport* exports)
{

}
int validate_Import(WasmImport* imports)
{

}