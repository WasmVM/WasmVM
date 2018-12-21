#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <stdint.h>
#include <stdlib.h>
#include <dataTypes/Value.h>
#include <dataTypes/stack.h>
#include <dataTypes/FuncType.h>
#include <structures/instrs/Control.h>
#include <structures/WasmFunc.h>
#include <structures/WasmModule.h>
#include <Validates.h>
#include <Opcodes.h>
#include <Context.h>
}
#undef _Bool

static void clean(stack* opds, stack* ctrls)
{
    for(stackNode* cur = opds->head; cur != NULL; cur = cur->next) {
        free(cur->data);
    }
    free_stack(opds);
    for(stackNode* cur = ctrls->head; cur != NULL; cur = cur->next) {
        free_ctrl_frame((ctrl_frame*)cur->data);
    }
    free_stack(ctrls);
}

SKYPAT_F(Validate_Instr_nop, valid)
{
    // Prepare
    WasmModule* module = new_WasmModule(NULL);
    WasmFunc* func = new_WasmFunc();
    FuncType* type = new_FuncType();
    module->types->push_back(module->types, type);
    Context* context = new_Context(module, func);
    stack* opds = new_stack();
    stack* ctrls = new_stack();
    ctrl_frame* frame = new_ctrl_frame();
    ctrls->push(ctrls, frame);

    WasmControlInstr* instr = (WasmControlInstr*)malloc(sizeof(WasmControlInstr));
    instr->parent.opcode = Op_nop;

    // Check
    EXPECT_EQ(validate_Instr_nop(instr, context, opds, ctrls), 0);

    // Clean
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}
