#ifndef UTIL_DEF
#define UTIL_DEF

#ifdef _WIN32
#define PATH_SEPARATOR "\\"
#else
#define PATH_SEPARATOR "/"
#endif

#include <cstdint>
#include <Exception.h>
#include <Bincode.h>

class Util{
public:
    static bool bigEndian;
    static void checkEndian();
    static std::uint16_t toLittle16(const std::uint16_t &val, bool force = false);
    static std::uint32_t toLittle32(const std::uint32_t &val, bool force = false);
    static std::uint64_t toLittle64(const std::uint64_t &val, bool force = false);
    static std::uint32_t getLeb128_u32(char* &ptr, const char *max);
    static std::int32_t getLeb128_i32(char* &ptr, const char *max);
    static std::int64_t getLeb128_i64(char* &ptr, const char *max);
    static std::uint32_t getLeb128_u32(std::vector<char> &funcBody, std::uint64_t &instrOffset);
    static std::int32_t getLeb128_i32(std::vector<char> &funcBody, std::uint64_t &instrOffset);
    static std::int64_t getLeb128_i64(std::vector<char> &funcBody, std::uint64_t &instrOffset);
    static std::uint64_t getContinueOffset(std::vector<char> &funcBody, Stack &coreStack, std::uint64_t instrOffset, bool stopElse = false);
};

#endif