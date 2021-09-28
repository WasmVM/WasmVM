#include "utils.h"

#include <error.h>

_Bool checkBigEndian()
{
    u32_t i = 0x12345678;
    // check
    if(*(char*) &i == 0x12) {
        return 1; // true, e.g. bigEndian
    } else {
        return 0; // false
    }
}

u16_t toLittle16(const u16_t val, _Bool force)
{
    if(force || checkBigEndian()) {
        return (val>>8) | (val<<8);
    } else {
        return val;
    }
}

u32_t toLittle32(const u32_t val, _Bool force)
{
    if(force || checkBigEndian()) {
        return (u32_t)toLittle16(val>>16, force) ||
               ((u32_t)toLittle16(val, force) << 16);
    } else {
        return val;
    }
}

u64_t toLittle64(const u64_t val, _Bool force)
{
    if(force || checkBigEndian()) {
        return (u64_t)toLittle32(val>>32, force) ||
               ((u64_t)toLittle32(val, force) << 32);
    } else {
        return val;
    }
}

u32_t getLeb128_u32(const byte_t **ptr, const byte_t* const end)
{
    // uleb128 reference: https://en.wikipedia.org/wiki/LEB128
    u32_t ret = 0;
    // Maximum: 5 bytes
    for(int i=0; i<5; ++i) {
        if(*ptr > end) {
            wasmvm_errno = ERROR_len_out_of_bound;
            return -1;
        }
        u32_t byte = *((*ptr)++);
        ret |= (byte & 0x7F) << (7*i);
        // check range
        if(i==4) {
            // last byte, check first bit
            if(byte & 0x80) {
                // means that there have more bytes after last byte
                // , which indicate that exceed 5 bytes
                // return error code
                wasmvm_errno = ERROR_int_rep_too_long;
                return -1;
            } else if(byte & 0x70) {
                wasmvm_errno = ERROR_int_too_large;
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

i32_t getLeb128_i32(const byte_t **ptr, const byte_t* const end)
{
    i32_t ret = 0;
    for(int i=0; i < 5; ++i) {
        if(*ptr > end) {
            wasmvm_errno = ERROR_len_out_of_bound;
            return -1;
        }
        i32_t byte = *((*ptr)++);
        ret |= (byte & 0x7F) << (7*i);
        // Check range
        if( i == 4 ) {
            if(byte & 0x80) {
                wasmvm_errno = ERROR_int_rep_too_long;
                return -1;
            } else {
                u8_t sign = byte & 0x08;
                if(sign) {
                    if((byte & 0xf0) != 0x70) {
                        wasmvm_errno = ERROR_int_too_large;
                        return -1;
                    }
                } else {
                    if((byte & 0xf0) != 0x00) {
                        wasmvm_errno = ERROR_int_too_large;
                        return -1;
                    }
                }
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

i64_t getLeb128_i64(const byte_t **ptr, const byte_t * const end)
{
    i64_t ret = 0;
    for(int i=0; i < 10; i++) {
        if(*ptr > end) {
            wasmvm_errno = ERROR_len_out_of_bound;
            return -1;
        }
        i64_t byte = *((*ptr)++);
        ret |= (byte & 0x7F) << (7*i);
        // Check range
        if(i==9) {
            if(byte & 0x80) {
                wasmvm_errno = ERROR_int_rep_too_long;
                return -1;
            } else {
                u8_t sign = byte & 0x01;
                if(sign) {
                    if((byte & 0xfe) != 0x7e) {
                        wasmvm_errno = ERROR_int_too_large;
                        return -1;
                    }
                } else {
                    if((byte & 0xfe) != 0x00) {
                        wasmvm_errno = ERROR_int_too_large;
                        return -1;
                    }
                }
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