#include <structures/instrs/Parametric.h>
#include <stdlib.h>

WasmParametricInstr* new_WasmParametricInstr()
{
    WasmParametricInstr* instr = (WasmParametricInstr*) malloc(sizeof(WasmParametricInstr));
    instr->parent.free = (void(*)(WasmInstr*)) free_WasmParametricInstr;
    instr->parent.clean = NULL;
    return instr;
}
void free_WasmParametricInstr(WasmParametricInstr* instr)
{
    free(instr);
}