#ifndef WASMVM_DATATYPES_CONSTEXPR_DEF
#define WASMVM_DATATYPES_CONSTEXPR_DEF

#include <dataTypes/Value.h>

typedef enum {
    Const_Unspecified = 0,
    Const_Value,
    Const_GlobalIndex,
} ConstType;

typedef struct {
    ConstType type;
    Value     value;
} ConstExpr;

#endif