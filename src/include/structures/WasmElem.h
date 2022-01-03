#ifndef WASMVM_STRUCTURE_ELEM
#define WASMVM_STRUCTURE_ELEM

#include <defines.h>
#include <dataTypes/Value.h>
#include <dataTypes/vector_t.h>
#include <dataTypes/ConstExpr.h>

typedef enum {
    Elem_unspecified = 0,
    Elem_passive,
    Elem_active,
    Elem_declarative,
} ElemMode;

typedef struct {
    ValueType           type;
    vector_t(ConstExpr) init;
    struct {
        ElemMode mode;
        u32_t        tableidx;
        ConstExpr    offset;
    } mode;
} WasmElem;

#endif