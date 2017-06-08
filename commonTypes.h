#ifndef COMMON_TYPES
#define COMMON_TYPES

#include <cstdint>

enum ValueType {i32, i64, u32, u64, f32, f64};

class Value{
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
#endif