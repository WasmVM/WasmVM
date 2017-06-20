#include "Core.h"
Core::Core(Memory &memory):
    memory(memory), running(false)
{
    
}

void Core::run(){
    running = true;
    Instruction::ctrl_call(memory.start_entry, opStack, locals, memory, pcStack);
    while(running){
        Decode::decode(memory, pcStack);
        running = false;
    }
    running = false;
}