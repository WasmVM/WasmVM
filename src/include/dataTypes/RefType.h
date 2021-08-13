#ifndef WASMVM_DATATYPES_REFTYPE_DEF
#define WASMVM_DATATYPES_REFTYPE_DEF

typedef enum {
    Ref_Unspecified = 0,
    Ref_func,
    Ref_extern,
} RefType;

#endif