#include "Utils.h"

#include <stdio.h>

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

uint32_t getLeb128_u32(uint8_t **ptr, const uint8_t* max)
{
    // uleb128 reference: https://en.wikipedia.org/wiki/LEB128
    uint32_t ret = 0;
    // Maximum: 5 bytes
    for(int i=0; i<5; ++i) {
        if(*ptr > max) {
            // error code
            return -1;
        }
        uint32_t byte = *((*ptr)++);
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

int32_t getLeb128_i32(uint8_t **ptr, const uint8_t* max)
{
    int32_t ret = 0;
    for(int i=0; i < 5; ++i) {
        if(*ptr > max) {
            printf("Pointer exceed max while decode leb128_32.\n");
            // error code
            return -1;
        }
        int32_t byte = *((*ptr)++);
        ret |= (byte & 0x7F) << (7*i);
        // Check range
        if( i == 4 ) {
            if(byte & 0x80) {
                printf("Too much bytes while decode leb128_32.\n");
                return -1;
            }
        }
        // Finished
        if((byte & 0x80) == 0) {
            if(byte & 0x40) {
                switch (i) {
                    case 0:
                        ret |= 0xFFFFFF80;
                        break;
                    case 1:
                        ret |= 0xFFFFC000;
                        break;
                    case 2:
                        ret |= 0xFFE00000;
                        break;
                    case 3:
                        ret |= 0xF0000000;
                        break;
                    default:
                        break;
                }
            }
            break;
        }
    }
    return toLittle32(ret, 0);
}

int64_t getLeb128_i64(uint8_t **ptr, const uint8_t *max)
{
    int64_t ret = 0;
    for(int i=0; i < 10; i++) {
        if(*ptr > max) {
            printf("Pointer exceed max while decode leb128_64.\n");
            // error code
            return -1;
        }
        int64_t byte = *((*ptr)++);
        ret |= (byte & 0x7F) << (7*i);
        // Check range
        if(i==9) {
            if(byte & 0x80) {
                printf("Too much bytes while decode leb128_64.\n");
                return -1;
            }
        }
        // Finished
        if((byte & 0x80) == 0) {
            if(byte & 0x40) {
                switch (i) {
                    case 0:
                        ret |= 0xFFFFFFFFFFFFFF80;
                        break;
                    case 1:
                        ret |= 0xFFFFFFFFFFFFC000;
                        break;
                    case 2:
                        ret |= 0xFFFFFFFFFFE00000;
                        break;
                    case 3:
                        ret |= 0xFFFFFFFFF0000000;
                        break;
                    case 4:
                        ret |= 0xFFFFFFF800000000;
                        break;
                    case 5:
                        ret |= 0xFFFFFC0000000000;
                        break;
                    case 6:
                        ret |= 0xFFFE000000000000;
                        break;
                    case 7:
                        ret |= 0xFF00000000000000;
                        break;
                    case 8:
                        ret |= 0x8000000000000000;
                        break;
                    default:
                        break;
                }
            }
            break;
        }
    }
    return toLittle64(ret, 0);
}