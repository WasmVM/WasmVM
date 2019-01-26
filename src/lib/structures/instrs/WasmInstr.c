#include <structures/instrs/WasmInstr.h>
#include <stddef.h>

void free_WasmInstr(WasmInstr* instr)
{
    if(instr != NULL) {
        instr->free(instr);
    }
}