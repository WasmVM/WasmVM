#ifndef CORE
#define CORE

#include "Memory.h"
#include "OperandStack.h"

class Core{
public:
    Core(Memory &memory);

    void run();

private:
    Memory &memory;
    OperandStack stack;
    uint64_t pc;
    bool running;
};

#endif