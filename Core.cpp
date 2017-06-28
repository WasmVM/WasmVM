#include "Core.h"

#include <cstdio>

Core::Core(Memory &memory):
    memory(memory), halted(false)
{
    
}

void Core::run(){
    try{
        halted = false;
        Instruction::ctrl_call(memory.start_entry, opStack, locals, memory);
        while(!halted){
            Decode::decode(memory, opStack, locals, halted);
        }
    }catch(char const *msg){
        printf("%s\n", msg);
    }
}