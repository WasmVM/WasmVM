#ifndef WASMVM_STACK_DEF
#define WASMVM_STACK_DEF

#include <defines.h>
#include <dataTypes/vector_t.h>
#include <dataTypes/Value.h>
#include <instances/ModuleInst.h>
#include <instances/InstrInst.h>

typedef struct {
    u32_t arity;
    wasm_module_inst moduleinst;
    vector_t(Value) locals;
} Frame;

typedef struct {
    u32_t arity;
    InstrInst* current;
    InstrInst* end;
} Label;

typedef enum {
    Entry_unspecified,
    Entry_frame,
    Entry_label,
    Entry_value,
} EntryType;

typedef struct _Stack{
    EntryType type;
    struct _Stack* next;
    union {
        Frame frame;
        Label label;
        Value value;
    } entry;
} Stack;

typedef Stack* wasm_stack;

#endif
