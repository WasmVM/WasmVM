#ifndef UTIL_DEF
#define UTIL_DEF

#ifdef _WIN32
#define PATH_SEPARATOR "\\"
#else
#define PATH_SEPARATOR "/"
#endif

#include <cstdint>
#include <string>
#include <Exception.h>
#include <Bincode.h>
#include <Stack.h>
#include <Store.h>

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
    static std::uint32_t getLeb128_u32(std::vector<char> &body, std::uint64_t &offset);
    static std::int32_t getLeb128_i32(std::vector<char> &body, std::uint64_t &offset);
    static std::int64_t getLeb128_i64(std::vector<char> &body, std::uint64_t &offset);
    static float getIEEE754_f32(std::vector<char> &body, std::uint64_t &offset);
    static double getIEEE754_f64(std::vector<char> &body, std::uint64_t &offset);
    static bool eqSign(float, float);
    static bool isZero(float);
    static bool isPos(float);
    static bool isNaN(float);
    static bool isInf(float);
    static bool eqSign(double, double);
    static bool isZero(double);
    static bool isPos(double);
    static bool isNaN(double);
    static bool isInf(double);
    static std::uint64_t getContinueOffset(std::vector<char> &funcBody, Stack &coreStack, std::uint64_t instrOffset, bool stopElse = false);
    static char *getMemoryPtr(Store &store, Stack &coreStack, std::uint32_t align, std::uint32_t offset, int byteWidth, std::string tag);
};

#endif