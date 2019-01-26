#include <structures/instrs/Variable.h>
#include <stdlib.h>

WasmVariableInstr* new_WasmVariableInstr(uint32_t index)
{
    WasmVariableInstr* instr = (WasmVariableInstr*) malloc(sizeof(WasmVariableInstr));
    instr->parent.free = (void(*)(WasmInstr*)) free_WasmVariableInstr;
    instr->index = index;
    return instr;
}
void free_WasmVariableInstr(WasmVariableInstr* instr)
{
    free(instr);
}