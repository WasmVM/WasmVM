#include "Validates.h"

int validate_Instr_const(WasmNumericInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    return 0;
}
int validate_Instr_unop(WasmNumericInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    return 0;
}
int validate_Instr_binop(WasmNumericInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    return 0;
}
int validate_Instr_testop(WasmNumericInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    return 0;
}
int validate_Instr_relop(WasmNumericInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    return 0;
}
int validate_Instr_cvtop(WasmNumericInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    return 0;
}
int validate_Instr_drop(WasmParametricInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    return 0;
}
int validate_Instr_select(WasmParametricInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    return 0;
}
int validate_Instr_get_local(WasmVariableInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    return 0;
}
int validate_Instr_set_local(WasmVariableInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    return 0;
}
int validate_Instr_tee_local(WasmVariableInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    return 0;
}
int validate_Instr_get_global(WasmVariableInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    return 0;
}
int validate_Instr_set_global(WasmVariableInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    return 0;
}
int validate_Instr_load(WasmMemoryInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    return 0;
}
int validate_Instr_loadN(WasmMemoryInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    return 0;
}
int validate_Instr_store(WasmMemoryInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    return 0;
}
int validate_Instr_storeN(WasmMemoryInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    return 0;
}
int validate_Instr_memory_size(WasmMemoryInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    return 0;
}
int validate_Instr_memory_grow(WasmMemoryInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    return 0;
}
int validate_Instr_nop(WasmControlInstr* instr, Context* context, stack* opds, stack* ctrls) //FIXME: Test
{
    return 0;
}
int validate_Instr_unreachable(WasmControlInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    // TODO:
    return 0;
}
int validate_Instr_block(WasmControlInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    ctrl_frame* frame = new_ctrl_frame();

    return 0;
}
int validate_Instr_loop(WasmControlInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    return 0;
}
int validate_Instr_if(WasmControlInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    return 0;
}
int validate_Instr_end(WasmControlInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    return 0;
}
int validate_Instr_else(WasmControlInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    return 0;
}
int validate_Instr_br(WasmControlInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    return 0;
}
int validate_Instr_br_if(WasmControlInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    return 0;
}
int validate_Instr_br_table(WasmControlInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    return 0;
}
int validate_Instr_return(WasmControlInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    return 0;
}
int validate_Instr_call(WasmControlInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    return 0;
}
int validate_Instr_call_indirect(WasmControlInstr* instr, Context* context, stack* opds, stack* ctrls)
{
    return 0;
}