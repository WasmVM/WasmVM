#include <structures/instrs/WasmInstr.h>
#include <stddef.h>

void clean_WasmInstr(WasmInstr* instr)
{
    if(instr->clean) {
        instr->clean(instr);
    }
}

void free_WasmInstr(WasmInstr* instr)
{
    if(instr != NULL) {
        instr->free(instr);
    }
}