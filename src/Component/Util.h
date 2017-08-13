#ifndef UTIL_DEF
#define UTIL_DEF

#ifdef _WIN32
#define PATH_SEPARATOR "\\"
#else
#define PATH_SEPARATOR "/"
#endif

#include <cstdint>
#include <Exception.h>

class Util{
public:
    static bool bigEndian;
    static void checkEndian();
    static std::uint16_t toLittle16(const std::uint16_t &val, bool force = false);
    static std::uint32_t toLittle32(const std::uint32_t &val, bool force = false);
    static std::uint64_t toLittle64(const std::uint64_t &val, bool force = false);
    static std::uint32_t get_uleb128_32(char* &ptr, const char *max);
    static std::int32_t get_leb128_32(char* &ptr, const char *max);
    static std::int64_t get_leb128_64(char* &ptr, const char *max);
    static std::uint32_t get_uleb128_32(std::vector<char> &funcBody, std::uint64_t &instrOffset);
    static std::int32_t get_leb128_32(std::vector<char> &funcBody, std::uint64_t &instrOffset);
    static std::int64_t get_leb128_64(std::vector<char> &funcBody, std::uint64_t &instrOffset);
};

#endif