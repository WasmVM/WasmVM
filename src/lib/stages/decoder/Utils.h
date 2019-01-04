#ifndef WASMVM_DECODER_UTILS_DEF
#define WASMVM_DECODER_UTILS_DEF

#include <stdint.h>

_Bool checkBigEndian();

uint16_t toLittle16(const uint16_t val, _Bool force);
uint32_t toLittle32(const uint32_t val, _Bool force);
uint64_t toLittle64(const uint64_t val, _Bool force);

uint32_t getLeb128_u32(char **ptr, const char **max);
int32_t getLeb128_i32(char **ptr, const char **max);
int64_t getLeb128_i64(char **ptr, const char **max);

#endif