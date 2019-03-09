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

void run_Core(Core *core, ModuleInst *thisModule, FuncInst *startFunc)
{
    core->moduleInst = thisModule;
    core->thisStack = new_Stack(NULL);
    // setup Frame of startFunc
    Frame *startFrame = new_Frame();
    memcpy(startFrame->moduleInst, thisModule);
    core->thisStack->entries->push(core->thisStack->entries, startFrame); //push the addres of Frame onto Stack
    
    // TODO save the next instruction after the current function finished to InstrInst *instrInst, but startFunc doesnt return 
    
    // TODO traverse all the instrInst in FuncInst->code
    vector *code = startFunc->code;
    size_t inst_len = code->length;
    InstrInst *cur_inst;
    for (int i = 0; i < inst_len; i++) {
        cur_inst = code->at(code, i);
        // process opcode
        switch(cur_inst->opcode) {
            // 收到 InstrInst 之後 opcode 得到 型態, 強制轉換成其他NumericInstrInst
        }
            
    }
}   