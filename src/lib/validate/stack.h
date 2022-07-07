#ifndef WASMVM_VALIDATE_STACK
#define WASMVM_VALIDATE_STACK

#include <defines.h>
#include <dataTypes/Value.h>
#include <dataTypes/vector_t.h>
#include <dataTypes/FuncType.h>

struct ValueNode;
struct CtrlStack;

typedef struct {
    size_t size;
    struct ValueNode* data;
} value_stack_t;

typedef struct CtrlStack* ctrl_stack_t;

typedef struct {
    u16_t opcode;
    _Bool unreachable;
    wasm_functype types;
    size_t height;
} ctrl_frame;

void push_val(value_stack_t vals, ValueType val);
ValueType pop_val(value_stack_t vals, ctrl_stack_t ctrls);
ValueType expect_val(value_stack_t vals, ctrl_stack_t ctrls, ValueType expect);
void free_value_stack(value_stack_t vals);

void push_ctrl(value_stack_t vals, ctrl_stack_t* ctrls, u16_t opcode, wasm_functype types);
ctrl_frame pop_ctrl(value_stack_t vals, ctrl_stack_t* ctrls);
void free_ctrl_stack(ctrl_stack_t* ctrls);
void free_frame(ctrl_frame frame);

void set_unreachable(value_stack_t vals, ctrl_stack_t ctrls);

#endif