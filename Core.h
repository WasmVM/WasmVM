#ifndef CORE
#define CORE

#include <stack>

#include "Memory.h"
#include "OperandStack.h"
#include "LocalStack.h"
#include "ControlInst.h"
#include "Decode.h"

using namespace std;

class Core{

public:
    Core(Memory &memory);
    void run();

private:
    Memory &memory;
    OperandStack opStack;
    LocalStack locals;
    stack<uint64_t> pcStack;
    bool running;
};

#endif