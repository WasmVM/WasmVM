#ifndef WASMVM_DATATYPES_REFTYPE_DEF
#define WASMVM_DATATYPES_REFTYPE_DEF

#include <defines.h>

typedef enum {
    Ref_Unspecified = 0,
    Ref_func,
    Ref_extern,
} RefType;

typedef struct {
    RefType type;
    _Bool isNull;
    u32_t addr;
} Ref;

typedef Ref wasm_ref;

#endif