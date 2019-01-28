#include <structures/instrs/Numeric.h>
#include <stdlib.h>

WasmNumericInstr* new_WasmNumericInstr()
{
    WasmNumericInstr* instr = (WasmNumericInstr*) malloc(sizeof(WasmNumericInstr));
    instr->parent.free = (void(*)(WasmInstr*)) free_WasmNumericInstr;
    instr->parent.clean = NULL;
    return instr;
}
void free_WasmNumericInstr(WasmNumericInstr* instr)
{
    free(instr);
}