#include <core/Core.h>
#include <stdint.h>
#include <stdlib.h>

Core* new_Core(Store *thisStore)
{
    Core *core = (Core *) malloc(sizeof(Core));
    core->thisStore = thisStore;

    return core;
}

void free_Core(Core* thisCore)
{
    // free the current Core only, since Store and Stack may be used by other core.
    free(thisCore);
}

void run_Core(Core *core, ModuleInst *thisModule, stack *thisStack, FuncInst *startFunc)
{
    core->moduleInst = thisModule;
    core->thisStack = thisStack; //maybe remove the arg thisStack and malloc a new Stack for Core here
    // setup Frame of startFunc
    core->thisFrame = (Frame *) malloc(sizeof(Frame));
    core->thisFrame->moduleInst = thisModule;
    core->thisFrame->localVals = startFunc->locals;
    
    // TODO save the next instruction after the current function finished to InstrInst *instrInst
    
    // TODO traverse all the instrInst in FuncInst->code
    vector *code = startFunc->code;
    size_t inst_len = code->length;
    InstrInst *cur_inst;
    for (int i = 0; i < inst_len; i++) {
        cur_inst = code->at(code, i);
        // process opcode
        switch(cur_inst->opcode) {

        }
            
    }
}   