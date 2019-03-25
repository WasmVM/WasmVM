#ifndef WASMVM_DECODER_UTILS_DEF
#define WASMVM_DECODER_UTILS_DEF

#include <stdint.h>

_Bool checkBigEndian();

uint16_t toLittle16(const uint16_t val, _Bool force);
uint32_t toLittle32(const uint32_t val, _Bool force);
uint64_t toLittle64(const uint64_t val, _Bool force);

uint32_t getLeb128_u32(uint8_t **ptr, const uint8_t* max);
int32_t getLeb128_i32(uint8_t **ptr, const uint8_t* max);
int64_t getLeb128_i64(uint8_t **ptr, const uint8_t* max);

#endif