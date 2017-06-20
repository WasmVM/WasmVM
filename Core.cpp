#include "Core.h"
Core::Core(Memory &memory):
    memory(memory), halted(false)
{
    
}

void Core::run(){
    halted = false;
    Instruction::ctrl_call(memory.start_entry, opStack, locals, memory);
    while(!halted){
        Decode::decode(memory, opStack, locals, halted);
    }
    halted = true;
}