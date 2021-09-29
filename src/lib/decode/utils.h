#ifndef WASMVM_DECODER_UTILS_DEF
#define WASMVM_DECODER_UTILS_DEF

#include <defines.h>

_Bool checkBigEndian();

u16_t toLittle16(const u16_t val, _Bool force);
u32_t toLittle32(const u32_t val, _Bool force);
u64_t toLittle64(const u64_t val, _Bool force);

u32_t getLeb128_u32(const byte_t **ptr, const byte_t* end);
i32_t getLeb128_i32(const byte_t **ptr, const byte_t* end);
i64_t getLeb128_i64(const byte_t **ptr, const byte_t* end);

_Bool check_utf8(const byte_t *ptr, const byte_t* end);

#endif