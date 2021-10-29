/* Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
rights reserved.
License to copy and use this software is granted provided that it
is identified as the "RSA Data Security, Inc. MD5 Message-Digest
Algorithm" in all material mentioning or referencing this software
or this function.
License is also granted to make and use derivative works provided
that such works are identified as "derived from the RSA Data
Security, Inc. MD5 Message-Digest Algorithm" in all material
mentioning or referencing the derived work.
RSA Data Security, Inc. makes no representations concerning either
the merchantability of this software or the suitability of this
software for any particular purpose. It is provided "as is"
without express or implied warranty of any kind.
These notices must be retained in any copies of any part of this
documentation and/or software.
*/

/**
 * Copyright 2021 Luis Hsu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include <helper.h>

#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

#define FF(a, b, c, d, x, s, ac) { \
    (a) += F ((b), (c), (d)) + (x) + (u32_t)(ac); \
    (a) = ROTATE_LEFT ((a), (s)); \
    (a) += (b); \
}
#define GG(a, b, c, d, x, s, ac) { \
    (a) += G ((b), (c), (d)) + (x) + (u32_t)(ac); \
    (a) = ROTATE_LEFT ((a), (s)); \
    (a) += (b); \
}
#define HH(a, b, c, d, x, s, ac) { \
    (a) += H ((b), (c), (d)) + (x) + (u32_t)(ac); \
    (a) = ROTATE_LEFT ((a), (s)); \
    (a) += (b); \
}
#define II(a, b, c, d, x, s, ac) { \
    (a) += I ((b), (c), (d)) + (x) + (u32_t)(ac); \
    (a) = ROTATE_LEFT ((a), (s)); \
    (a) += (b); \
}

typedef union {
    u32_t code[16];
    u8_t data[64];
    struct {
        u8_t data[56];
        u64_t count;
    } padding;
} Chunk;

static void md5_transform(u32_t state[4], Chunk chunk)
{
    u32_t a = state[0], b = state[1], c = state[2], d = state[3];
    /* Round 1 */
    FF(a, b, c, d, chunk.code[ 0], 7, 0xd76aa478);
    FF(d, a, b, c, chunk.code[ 1], 12, 0xe8c7b756);
    FF(c, d, a, b, chunk.code[ 2], 17, 0x242070db);
    FF(b, c, d, a, chunk.code[ 3], 22, 0xc1bdceee);
    FF(a, b, c, d, chunk.code[ 4], 7, 0xf57c0faf);
    FF(d, a, b, c, chunk.code[ 5], 12, 0x4787c62a);
    FF(c, d, a, b, chunk.code[ 6], 17, 0xa8304613);
    FF(b, c, d, a, chunk.code[ 7], 22, 0xfd469501);
    FF(a, b, c, d, chunk.code[ 8], 7, 0x698098d8);
    FF(d, a, b, c, chunk.code[ 9], 12, 0x8b44f7af);
    FF(c, d, a, b, chunk.code[10], 17, 0xffff5bb1);
    FF(b, c, d, a, chunk.code[11], 22, 0x895cd7be);
    FF(a, b, c, d, chunk.code[12], 7, 0x6b901122);
    FF(d, a, b, c, chunk.code[13], 12, 0xfd987193);
    FF(c, d, a, b, chunk.code[14], 17, 0xa679438e);
    FF(b, c, d, a, chunk.code[15], 22, 0x49b40821);
    /* Round 2 */
    GG(a, b, c, d, chunk.code[ 1], 5, 0xf61e2562);
    GG(d, a, b, c, chunk.code[ 6], 9, 0xc040b340);
    GG(c, d, a, b, chunk.code[11], 14, 0x265e5a51);
    GG(b, c, d, a, chunk.code[ 0], 20, 0xe9b6c7aa);
    GG(a, b, c, d, chunk.code[ 5], 5, 0xd62f105d);
    GG(d, a, b, c, chunk.code[10], 9, 0x02441453);
    GG(c, d, a, b, chunk.code[15], 14, 0xd8a1e681);
    GG(b, c, d, a, chunk.code[ 4], 20, 0xe7d3fbc8);
    GG(a, b, c, d, chunk.code[ 9], 5, 0x21e1cde6);
    GG(d, a, b, c, chunk.code[14], 9, 0xc33707d6);
    GG(c, d, a, b, chunk.code[ 3], 14, 0xf4d50d87);
    GG(b, c, d, a, chunk.code[ 8], 20, 0x455a14ed);
    GG(a, b, c, d, chunk.code[13], 5, 0xa9e3e905);
    GG(d, a, b, c, chunk.code[ 2], 9, 0xfcefa3f8);
    GG(c, d, a, b, chunk.code[ 7], 14, 0x676f02d9);
    GG(b, c, d, a, chunk.code[12], 20, 0x8d2a4c8a);
    /* Round 3 */
    HH(a, b, c, d, chunk.code[ 5], 4, 0xfffa3942);
    HH(d, a, b, c, chunk.code[ 8], 11, 0x8771f681);
    HH(c, d, a, b, chunk.code[11], 16, 0x6d9d6122);
    HH(b, c, d, a, chunk.code[14], 23, 0xfde5380c);
    HH(a, b, c, d, chunk.code[ 1], 4, 0xa4beea44);
    HH(d, a, b, c, chunk.code[ 4], 11, 0x4bdecfa9);
    HH(c, d, a, b, chunk.code[ 7], 16, 0xf6bb4b60);
    HH(b, c, d, a, chunk.code[10], 23, 0xbebfbc70);
    HH(a, b, c, d, chunk.code[13], 4, 0x289b7ec6);
    HH(d, a, b, c, chunk.code[ 0], 11, 0xeaa127fa);
    HH(c, d, a, b, chunk.code[ 3], 16, 0xd4ef3085);
    HH(b, c, d, a, chunk.code[ 6], 23, 0x04881d05);
    HH(a, b, c, d, chunk.code[ 9], 4, 0xd9d4d039);
    HH(d, a, b, c, chunk.code[12], 11, 0xe6db99e5);
    HH(c, d, a, b, chunk.code[15], 16, 0x1fa27cf8);
    HH(b, c, d, a, chunk.code[ 2], 23, 0xc4ac5665);
    /* Round 4 */
    II(a, b, c, d, chunk.code[ 0], 6, 0xf4292244);
    II(d, a, b, c, chunk.code[ 7], 10, 0x432aff97);
    II(c, d, a, b, chunk.code[14], 15, 0xab9423a7);
    II(b, c, d, a, chunk.code[ 5], 21, 0xfc93a039);
    II(a, b, c, d, chunk.code[12], 6, 0x655b59c3);
    II(d, a, b, c, chunk.code[ 3], 10, 0x8f0ccc92);
    II(c, d, a, b, chunk.code[10], 15, 0xffeff47d);
    II(b, c, d, a, chunk.code[ 1], 21, 0x85845dd1);
    II(a, b, c, d, chunk.code[ 8], 6, 0x6fa87e4f);
    II(d, a, b, c, chunk.code[15], 10, 0xfe2ce6e0);
    II(c, d, a, b, chunk.code[ 6], 15, 0xa3014314);
    II(b, c, d, a, chunk.code[13], 21, 0x4e0811a1);
    II(a, b, c, d, chunk.code[ 4], 6, 0xf7537e82);
    II(d, a, b, c, chunk.code[11], 10, 0xbd3af235);
    II(c, d, a, b, chunk.code[ 2], 15, 0x2ad7d2bb);
    II(b, c, d, a, chunk.code[ 9], 21, 0xeb86d391);
    // Update
    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
}

void md5_hash(const u32_t data_size, const byte_t data[data_size], u64_t result[2])
{
    // Init
    u32_t state[4] = {
        0x67452301,
        0xefcdab89,
        0x98badcfe,
        0x10325476,
    };
    Chunk chunk;
    // Update
    const byte_t* cursor = data;
    for(size_t i = 0; (i + 64) < data_size; i += 64) {
        // Copy data
        for(size_t j = 0; j < 64; ++j) {
            chunk.data[j] = *(cursor++);
        }
        md5_transform(state, chunk);
    }
    // Padding
    size_t remaining = data_size % 64;
    // Fill into chunk
    for(size_t i = 0; i < remaining; ++i) {
        chunk.data[i] = *(cursor++);
    }
    chunk.data[remaining++] = 0x80;
    if(remaining > 56) {
        for(size_t i = remaining; i < 64; ++i) {
            chunk.data[i] = 0;
        }
        md5_transform(state, chunk);
        remaining = 0;
    }
    for(size_t i = remaining; i < 56; ++i) {
        chunk.padding.data[i] = 0;
    }
    chunk.padding.count = data_size * 8;
    md5_transform(state, chunk);
    // Update result
    u32_t* resultPtr = (u32_t*) result;
    resultPtr[0] = state[0];
    resultPtr[1] = state[1];
    resultPtr[2] = state[2];
    resultPtr[3] = state[3];
}