#ifndef WASMVM_DATATYPES_DESCTYPE_DEF
#define WASMVM_DATATYPES_DESCTYPE_DEF

typedef enum {
    Desc_Unspecified = 0,
    Desc_Func,
    Desc_Table,
    Desc_Mem,
    Desc_Global
} DescType;

#endif