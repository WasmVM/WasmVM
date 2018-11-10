#include <Utils.h>

_Bool checkBigEndian()
{
    uint32_t i = 0x12345678;
    // check
    if(*(char*) &i == 0x12) {
        return 1; // true, e.g. bigEndian
    } else {
        return 0; // false
    }
}

uint16_t toLittle16(const uint16_t val, _Bool force)
{
    if(force || checkBigEndian()) {
        return (val>>8) | (val<<8);
    } else {
        return val;
    }
}

uint32_t toLittle32(const uint32_t val, _Bool force)
{
    if(force || checkBigEndian()) {
        return (uint32_t)toLittle16(val>>16, force) ||
               ((uint32_t)toLittle16(val, force) << 16);
    } else {
        return val;
    }
}

uint64_t toLittle64(const uint64_t val, _Bool force)
{
    if(force || checkBigEndian()) {
        return (uint64_t)toLittle32(val>>32, force) ||
               ((uint64_t)toLittle32(val, force) << 32);
    } else {
        return val;
    }
}

uint32_t getLeb128_u32(char* ptr, const char* max)
{
    // uleb128 reference: https://en.wikipedia.org/wiki/LEB128
    uint32_t ret = 0;
    // Maximum: 5 bytes
    for(int i=0; i<5; ++i) {
        if(ptr > max) {
            // error code
            return -1;
        }
        uint32_t byte = *(ptr++);
        ret |= (byte & 0x7F) << (7*i);
        // check range
        if(i==4) {
            // last byte, check first bit
            if(byte & 0x80) {
                // means that there have more bytes after last byte
                // , which indicate that exceed 5 bytes
                // return error code
                return -1;
            }
        }
        // Finished
        if((byte & 0x80) == 0) {
            break;
        }
    }

    return toLittle32(ret, 0);
}