#ifndef DECODE
#define DECODE

#include <cstdint>
#include <stack>

#include "Memory.h"

namespace Decode{
    void decode(Memory &memory, stack<uint64_t> pcStack);
    uint32_t get_uleb128_32(Memory &memory, uint64_t &pc);
    int32_t get_leb128_32(Memory &memory, uint64_t &pc);
};
#endif