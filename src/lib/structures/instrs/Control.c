#include <structures/instrs/Control.h>

#include <stdlib.h>

WasmControlInstr* new_WasmControlInstr()
{
    WasmControlInstr* newInstr = (WasmControlInstr*)malloc(sizeof(WasmControlInstr));
    newInstr->parent.free = (void(*)(WasmInstr*)) free_WasmControlInstr;
    newInstr->parent.clean = clean_WasmControlInstr;
    newInstr->resultTypes = new_vector(sizeof(ValueType), NULL);
    newInstr->instrs = new_vector(sizeof(ValueType), NULL);
    newInstr->indices = new_vector(sizeof(ValueType), NULL);
    return newInstr;
}

void clean_WasmControlInstr(WasmControlInstr* thisInstr)
{
    if(thisInstr != NULL) {
        free_vector(thisInstr->resultTypes);
        free_vector(thisInstr->instrs);
        free_vector(thisInstr->indices);
    }
}

void free_WasmControlInstr(WasmControlInstr* thisInstr)
{
    clean_WasmControlInstr(thisInstr);
    free(thisInstr);
}
