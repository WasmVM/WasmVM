#ifndef UTIL_DEF
#define UTIL_DEF

#include <cstdint>

class Util{
public:
    static bool bigEndian;
    static void checkEndian();
    static std::uint16_t toLittle16(const std::uint16_t &val, bool force = false);
    static std::uint32_t toLittle32(const std::uint32_t &val, bool force = false);
    static std::uint64_t toLittle64(const std::uint64_t &val, bool force = false);
};

#endif