#ifndef DECODE
#define DECODE

#include <cstdint>
#include <stack>

#include "Memory.h"
#include "OperandStack.h"
#include "LocalStack.h"
#include "NumericInst.h"
#include "ParaVarInst.h"
#include "ControlInst.h"

namespace Decode{
    void decode(Memory &memory, OperandStack &opStack, LocalStack &locals, bool &halted);
};
#endif