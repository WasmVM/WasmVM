#ifndef WASMVM_DEFINES_DEF
#define WASMVM_DEFINES_DEF

typedef signed int          i32_t;
typedef signed long         i64_t;
typedef unsigned int        u32_t;
typedef unsigned long long  u64_t;
typedef float   f32_t;
typedef double  f64_t;

typedef unsigned char   u8_t;
typedef unsigned short  u16_t;
typedef unsigned long   size_t;
typedef unsigned char   byte_t;

#include <stdlib.h>
#include <string.h>
#define malloc_func malloc
#define free_func free
#define realloc_func realloc
#define memset_func memset

#define memcpy_func(D, S, Siz) _memcpy_func((char*)(D), (const char*)(S), (Siz))
void _memcpy_func(char* const dst, const char* const src, u64_t size);

#endif