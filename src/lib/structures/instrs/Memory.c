#include <structures/instrs/Memory.h>
#include <stdlib.h>

WasmMemoryInstr* new_WasmMemoryInstr(uint32_t offset, uint32_t align)
{
    WasmMemoryInstr* instr = (WasmMemoryInstr*) malloc(sizeof(WasmMemoryInstr));
    instr->parent.free = (void(*)(WasmInstr*))free_WasmMemoryInstr;
    instr->offset = offset;
    instr->align = align;
    return instr;
}
void free_WasmMemoryInstr(WasmMemoryInstr* instr)
{
    free(instr);
}