#ifndef WASMVM_STRUCTURE_DATA
#define WASMVM_STRUCTURE_DATA

#include <defines.h>
#include <dataTypes/vector_t.h>
#include <dataTypes/ConstExpr.h>

typedef enum {
    Data_unspecified = 0,
    Data_passive,
    Data_active,
} DataMode;

typedef struct {
    vector_t(byte_t) init;
    struct {
        DataMode    mode;
        u32_t       memidx;
        ConstExpr   offset;
    } mode;
} WasmData;

#endif