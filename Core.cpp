#include "Core.h"
Core::Core(Memory &memory):
    memory(memory), running(false)
{

}

void Core::run(){
    running = true;
    while(running){
        // TODO: Do Fetsh & Decode here
        Memory::dump(memory);
        running = false;
    }
}