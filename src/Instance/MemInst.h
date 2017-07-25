#ifndef MEMINST_DEF
#define MEMINST_DEF

#include <cstdint>
#include <vector>

class MemInst{
public:
    std::vector<char> data;
    uint32_t max;
};

#endif