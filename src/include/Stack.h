#ifndef WASMVM_STACK_DEF
#define WASMVM_STACK_DEF

#include <defines.h>
#include <dataTypes/vector_t.h>
#include <dataTypes/Value.h>
#include <instances/ModuleInst.h>
#include <instances/InstrInst.h>

typedef struct {
    u32_t arity;
    vector_t(Value) locals;
    wasm_module_inst moduleinst;
} Frame;

typedef struct {
    u32_t arity;
    InstrInst* current, end;
} Label;

typedef enum {
    Entry_unspecified,
    Entry_frame,
    Entry_label,
    Entry_value,
} EntryType;

typedef struct _Stack{
    EntryType type;
    union {
        Frame frame;
        Label label;
        Value value;
    } entry;
    struct _Stack* next;
} Stack;

#endif
