#ifndef COMMON_TYPES
#define COMMON_TYPES

#include <cstdint>
#include "Memory.h"

enum ValueType {i32, i64, f32, f64};

class Value{
public:
  union{
    int32_t  i32;
    int64_t  i64;
    uint32_t u32;
    uint64_t u64;
    float    f32;
    double   f64;
  } data;
  ValueType type;
};

namespace Common{
  // LEB128
  uint32_t get_uleb128_32(Memory &memory, uint64_t &pc);
  int32_t get_leb128_32(Memory &memory, uint64_t &pc);
  int64_t get_leb128_64(Memory &memory, uint64_t &pc);
};


#endif