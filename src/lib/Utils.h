#ifndef UTILS
#define UTILS 

#include <stdint.h>

_Bool checkBigEndian();
uint16_t toLittle16(const uint16_t val, _Bool force);
uint32_t toLittle32(const uint32_t val, _Bool force);
uint64_t toLittle64(const uint64_t val, _Bool force);


#endif