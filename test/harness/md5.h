#ifndef WASMVM_TEST_MD5_DEF
#define WASMVM_TEST_MD5_DEF

#include <defines.h>

void md5_hash(const u32_t data_size, const byte_t data[data_size], u64_t result[2]);

#endif