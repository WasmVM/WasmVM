#include <structures/instrs/Control.h>

#include <stdlib.h>

WasmControlInstr* new_WasmControlInstr()
{
    WasmControlInstr* newInstr = (WasmControlInstr*)malloc(sizeof(WasmControlInstr));

    newInstr->resultTypes = new_vector(sizeof(ValueType));
    newInstr->instrs = new_vector(sizeof(ValueType));
    newInstr->indices = new_vector(sizeof(ValueType));

    return newInstr;
}

void free_WasmControlInstr(WasmControlInstr* thisInstr)
{
    free_vector(thisInstr->resultTypes);
    free_vector(thisInstr->instrs);
    free_vector(thisInstr->indices);
    free(thisInstr);
}
