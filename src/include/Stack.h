#ifndef WASMVM_STACK_DEF
#define WASMVM_STACK_DEF

#include <defines.h>
#include <dataTypes/vector_t.h>
#include <dataTypes/Value.h>
#include <instances/ModuleInst.h>
#include <instances/InstrInst.h>

struct _Stack;

typedef struct _Frame{
    u32_t arity;
    wasm_module_inst moduleinst;
    struct _Stack* last;
    vector_t(Value) locals;
} Frame;

typedef struct _Label{
    u32_t arity;
    InstrInst* current;
    InstrInst* branch;
    struct _Stack* last;
} Label;

typedef enum {
    Entry_unspecified,
    Entry_frame,
    Entry_label,
    Entry_value,
} EntryType;

typedef struct _Stack{
    EntryType type;
    struct _Stack *next;
    union {
        Frame frame;
        Label label;
        Value value;
    } entry;
} Stack;

typedef Stack* wasm_stack;

#endif
