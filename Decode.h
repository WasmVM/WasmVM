#ifndef DECODE
#define DECODE

#include <cstdint>
#include <stack>

#include "Memory.h"
#include "OperandStack.h"
#include "LocalStack.h"
#include "NumericInst.h"
#include "ParaVarInst.h"

namespace Decode{
    void decode(Memory &memory, OperandStack &opStack, LocalStack &locals, bool &halted);
    // LEB128
    uint32_t get_uleb128_32(Memory &memory, uint64_t &pc);
    int32_t get_leb128_32(Memory &memory, uint64_t &pc);
    int64_t get_leb128_64(Memory &memory, uint64_t &pc);
};
#endif