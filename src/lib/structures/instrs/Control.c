#include <structures/instrs/Control.h>

#include <stdlib.h>

WasmControlInstr* new_WasmControlInstr()
{
    WasmControlInstr* newInstr = (WasmControlInstr*)malloc(sizeof(WasmControlInstr));
    newInstr->parent.free = (void(*)(WasmInstr*)) free_WasmControlInstr;
    newInstr->parent.clean = (void(*)(WasmInstr*)) clean_WasmControlInstr;
    newInstr->resultTypes = new_vector_p(ValueType, NULL);
    newInstr->instrs = new_vector_p(ValueType, NULL);
    newInstr->indices = new_vector_p(ValueType, NULL);
    return newInstr;
}

void clean_WasmControlInstr(WasmControlInstr* thisInstr)
{
    if(thisInstr != NULL) {
        free_vector_p(thisInstr->resultTypes);
        free_vector_p(thisInstr->instrs);
        free_vector_p(thisInstr->indices);
    }
}

void free_WasmControlInstr(WasmControlInstr* thisInstr)
{
    clean_WasmControlInstr(thisInstr);
    free(thisInstr);
}
